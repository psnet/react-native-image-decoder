import { NativeModules } from 'react-native';

// perform lazy init
NativeModules.ImageDecoderModule;

export type DecodedImage = {
    data: Uint8Array;
    width: number;
    height: number;
};

export function decodeImage(uint8Array: Uint8Array): DecodedImage {
    const globalAny = (global as any);
    const bridge = globalAny.RNImageDecoder;

    if (!bridge || typeof bridge.decodeImage !== 'function') {
        throw new Error('RNImageDecoder JSI function not installed. Make sure the native module is properly autolinked and initialized.');
    }

    const result = bridge.decodeImage(uint8Array);

    if (!result || !(result.data instanceof Uint8Array) || typeof result.width !== 'number' || typeof result.height !== 'number') {
        throw new Error('Invalid result from native decodeImage');
    }

    return result as DecodedImage;
}
