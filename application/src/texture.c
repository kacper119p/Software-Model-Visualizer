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
#include "texture.h"

#include "stb_image.h"
#include <string.h>

enum LoadTextureResult loadTexture(const char* const FilePath,
                                   struct Texture* const Destination) {
  if (FilePath == nullptr || Destination == nullptr) {
    return LOAD_TEXTURE_RESULT_INVALID_TARGET;
  }
  Destination->Data =
      stbi_load(FilePath, &Destination->Width, &Destination->Height,
                &Destination->Channels, 0);
  if (Destination->Data != nullptr) {
    return LOAD_TEXTURE_RESULT_SUCCESS;
  }
  const char* const reason = stbi_failure_reason();
  if (strstr(reason, "can't fopen") != nullptr) {
    return LOAD_TEXTURE_RESULT_FILE_NOT_FOUND;
  }
  if (strstr(reason, "out of memory") != nullptr) {
    return LOAD_TEXTURE_RESULT_OUT_OF_MEMORY;
  }
  return LOAD_TEXTURE_RESULT_UNSUPPORTED_FORMAT;
}

void destroyTexture(const struct Texture* const Texture) {
  stbi_image_free(Texture->Data);
}
