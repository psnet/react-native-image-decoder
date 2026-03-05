# React Native Image Decoder

Android-only **fast** `JSI C++` **image decoder** (TS/JS API) with zero-copy input/output using `stb_image` for decoding `PNG/JPEG` to `RGBA` with **zero dependencies**.

Target
---

**React Native** >= 0.81.5 with Hermes.

Works with **Expo**.

Installation
---

```
npm i https://github.com/psnet/react-native-image-decoder
```

API
---

```ts
import {decodeImage} from 'react-native-image-decoder';

const decodedImageData = decodeImage(uint8Array);
```
`decodedImageData` has `DecodedImage` interface:
```ts
{
    data: Uint8Array;
    width: number;
    height: number;
}
```

Features
---

- **Fast**: uses **C++ JSI**
- **Zero-copy input**: reads `Uint8Array.buffer` directly via JSI with respecting `byteOffset` and `byteLength`
- **Zero-copy output**: returns a `Uint8Array` that points to the decoded **RGBA** `ArrayBuffer`
- **Zero dependencies**
- **No precompiled binary code included**, compilation will be done during app build, source code can be easily inspected
- **Small size**
- **TypeScript** API for RN
