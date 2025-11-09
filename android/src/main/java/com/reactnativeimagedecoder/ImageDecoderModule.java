package com.reactnativeimagedecoder;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.JavaScriptContextHolder;

public class ImageDecoderModule extends ReactContextBaseJavaModule {
    static {
        System.loadLibrary("rn_imagedecoder");
    }

    private final ReactApplicationContext reactContext;

    public ImageDecoderModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
    }

    @Override
    public String getName() {
        return "ImageDecoderModule";
    }

    @Override
    public void initialize() {
        super.initialize();

        try {
            JavaScriptContextHolder holder = (JavaScriptContextHolder) reactContext.getJavaScriptContextHolder();
            long jsContext = holder.get();
            
            nativeInstall(jsContext);
        } catch (Exception e) {
            // ignore if JS context unavailable
        }
    }

    private static native void nativeInstall(long jsContextPointer);
}
