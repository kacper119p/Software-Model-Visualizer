/* MIT License
 *
 * Copyright (c) 2026 Kacper Pastuszka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef SOFTWARE_MODEL_VISUALIZER_TEXTURE_H
#define SOFTWARE_MODEL_VISUALIZER_TEXTURE_H

#include "graphicsMath.h"

#include <stdint.h>

struct Texture {
  uint8_t* Data;
  int Width;
  int Height;
  int Channels;
};

enum LoadTextureResult {
  LOAD_TEXTURE_RESULT_SUCCESS = 0,
  LOAD_TEXTURE_RESULT_FILE_NOT_FOUND = 1,
  LOAD_TEXTURE_RESULT_UNSUPPORTED_FORMAT = 2,
  LOAD_TEXTURE_RESULT_OUT_OF_MEMORY = 3,
  LOAD_TEXTURE_RESULT_INVALID_TARGET = 4
};

enum LoadTextureResult loadTexture(const char* FilePath,
                                   struct Texture* Destination);
void destroyTexture(const struct Texture* Texture);

static inline struct Color textureSample(const struct Texture* Texture,
                                         const struct Vec2 TextureCoords) {
  const float u = TextureCoords.X - (int)TextureCoords.X +
                  (TextureCoords.X < 0.0f ? 1.0f : 0.0f);
  const float v = TextureCoords.Y - (int)TextureCoords.Y +
                  (TextureCoords.Y < 0.0f ? 1.0f : 0.0f);
  const int x = (int)(u * (float)Texture->Width) % Texture->Width;
  const int y = (int)(v * (float)Texture->Height) % Texture->Height;
  const uint8_t* p =
      Texture->Data + (y * Texture->Width + x) * Texture->Channels;
  return (struct Color){p[0], p[1], Texture->Channels >= 3 ? p[2] : p[1]};
}

#endif // SOFTWARE_MODEL_VISUALIZER_TEXTURE_H
