import {NativeModules} from 'react-native';

// perform lazy init
NativeModules.SepuImageDecoderModule;

export interface DecodedImage {
    data: Uint8Array;
    width: number;
    height: number;
};

/**
 * Decodes PNG/JPEG image into RGBA `Uint8Array`
 */
export function decodeImage(uint8Array: Uint8Array): DecodedImage {
    const globalAny = (globalThis as any);
    const nativeModule = globalAny.SepuImageDecoder;

    if (!nativeModule || typeof nativeModule.decodeImage !== 'function') {
        throw new Error('SepuImageDecoder JSI function is not installed. Make sure the native module is properly autolinked and initialized.');
    }

    const result = nativeModule.decodeImage(uint8Array);

    if (!result || !(result.data instanceof Uint8Array) || typeof result.width !== 'number' || typeof result.height !== 'number') {
        throw new Error('Invalid result from native decodeImage');
    }

    return result as DecodedImage;
}
