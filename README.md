# React Native Image Decoder

### React Native module

Android-only **fast** `JSI C++` **image decoder** (JS API) zero-copy input/output using `stb_image` for decoding `PNG/JPEG` to `RGBA` with zero dependencies.

**Target:** React Native >= 0.81.5 with Hermes.

### API

```ts
import { decodeImage } from 'react-native-image-decoder';

const result = decodeImage(new Uint8Array(byteArray));
// result: { data: Uint8Array (RGBA), width: number, height: number }
```

### Implementation

- **Zero-copy input**: reads `Uint8Array.buffer` directly via JSI `ArrayBuffer::data()` (with respecting byte offset and byte length of view) without copying the compressed bytes.
- **Zero-copy output**: returns a `Uint8Array` points to the decoded RGBA buffer. Implementation ties lifetime to a `std::shared_ptr` that frees via `stbi_image_free()`.
- Uses `stb_image` to decode compressed bytes to RGBA (`STBI_rgb_alpha`).
