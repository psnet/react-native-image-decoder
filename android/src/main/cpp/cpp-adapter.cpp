#include <jni.h>
#include <jsi/jsi.h>
#include <memory>
#include <vector>
#include <string>
#include <cstring>

// https://github.com/nothings/stb/blob/master/stb_image.h
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace facebook;

static jsi::Runtime* runtimeFromPointer(jlong ptr) {
    return reinterpret_cast<jsi::Runtime*>(ptr);
}

namespace {
    class ExternalBuffer : public jsi::MutableBuffer {
        public:

            ExternalBuffer(std::shared_ptr<uint8_t> data, size_t size)
                : data_(std::move(data)), size_(size) {}

            size_t size() const override {
                return size_;
            }

            uint8_t* data() override {
                return data_.get();
            }

        private:
        
            std::shared_ptr<uint8_t> data_;
            size_t size_;
    };
}

static jsi::ArrayBuffer createZeroCopyArrayBuffer(jsi::Runtime& rt,
                                                  std::shared_ptr<uint8_t> data,
                                                  size_t size) {
    auto buffer = std::make_shared<ExternalBuffer>(data, size);

    return jsi::ArrayBuffer(rt, buffer);
}


/**
 * decodeImage_jsi
 * - Zero-copy input: reads compressed bytes from incoming Uint8Array.buffer via ArrayBuffer::data()/size()
 * - Uses stb_image to decode into RGBA (4 channels)
 * - Wraps decoded pixels into a host-backed ArrayBuffer (best-effort) and returns a Uint8Array view
 */
static jsi::Value decodeImage_jsi(jsi::Runtime& rt,
                                  const jsi::Value& thisVal,
                                  const jsi::Value* args,
                                  size_t count) {
    if (count < 1 || !args[0].isObject()) {
        throw jsi::JSError(rt, "decodeImage requires a Uint8Array as first argument");
    }

    jsi::Object incoming = args[0].asObject(rt);
    // Get the .buffer property (ArrayBuffer) from the Uint8Array
    jsi::Value bufferVal = incoming.getProperty(rt, "buffer");

    if (bufferVal.isUndefined() || !bufferVal.isObject()) {
        throw jsi::JSError(rt, "Failed to get ArrayBuffer from Uint8Array");
    }

    jsi::Object bufferObj = bufferVal.asObject(rt);
    jsi::ArrayBuffer arrayBuffer = bufferObj.getArrayBuffer(rt);

    // Zero-copy access to input bytes

    // Uint8Array byteOffset and byteLength
    size_t offset = incoming.getProperty(rt, "byteOffset").asNumber();
    size_t length = incoming.getProperty(rt, "byteLength").asNumber();
    
    uint8_t* inputPtr = static_cast<uint8_t*>(arrayBuffer.data(rt)) + offset;
    size_t inputLen = length;

    if (!inputPtr || inputLen == 0) {
        throw jsi::JSError(rt, "Empty input ArrayBuffer");
    }

    // Decode using stb_image (force 4 channels = RGBA)
    int width = 0, height = 0, channels_in_file = 0;
    const int desired_channels = 4;

    unsigned char* decoded = stbi_load_from_memory((const unsigned char*)inputPtr, (int)inputLen, &width, &height, &channels_in_file, desired_channels);
    
    if (!decoded) {
        throw jsi::JSError(
            rt,
            std::string("stb_image failed to decode image (") +
            (stbi_failure_reason() ? stbi_failure_reason() : "unknown error") +
            ")"
        );
    }

    size_t pixelsByteLength = (size_t)width * (size_t)height * (size_t)desired_channels;

    // Wrap decoded pixels into a shared_ptr with custom deleter that calls stbi_image_free.
    std::shared_ptr<uint8_t> pixels(decoded, [](uint8_t* p) {
        if (p) stbi_image_free(p);
    });

    jsi::ArrayBuffer outBuffer = createZeroCopyArrayBuffer(rt, pixels, pixelsByteLength);

    // Create a Uint8Array view over the ArrayBuffer
    jsi::Function uint8Ctor = rt.global().getPropertyAsFunction(rt, "Uint8Array");
    jsi::Object dataArray = uint8Ctor.callAsConstructor(rt, outBuffer).asObject(rt);

    jsi::Object result(rt);
    result.setProperty(rt, "data", dataArray);
    result.setProperty(rt, "width", (double)width);
    result.setProperty(rt, "height", (double)height);

    return jsi::Value(rt, result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativeimagedecoder_ImageDecoderModule_nativeInstall(JNIEnv* env, jclass, jlong jsContextPtr) {
    jsi::Runtime* runtime = runtimeFromPointer(jsContextPtr);

    if (!runtime) {
        // Failed to get jsi::Runtime from pointer
        return;
    }

    jsi::Runtime& rt = *runtime;

    jsi::Object rnObj(rt);

    auto decodeFunc = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "decodeImage"),
        2,
        decodeImage_jsi
    );

    rnObj.setProperty(rt, "decodeImage", decodeFunc);

    rt.global().setProperty(rt, "RNImageDecoder", rnObj);
}
