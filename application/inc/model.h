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
#ifndef SOFTWARE_MODEL_VISUALIZER_MODEL_H
#define SOFTWARE_MODEL_VISUALIZER_MODEL_H
#include "graphicsMath.h"

#include <stddef.h>
#include <stdint.h>

struct Model {
  struct Vec3* Vertices;
  size_t VertexCount;
  uint32_t* Indices;
  size_t IndexCount;
  struct Vec3* Normals;
  struct Vec2* TextureCoords;
  uint32_t* Colors;
  struct Vec3 AabbMin;
  struct Vec3 AabbMax;
};

enum LoadModelResult {
  LOAD_MODEL_RESULT_SUCCESS = 0,
  LOAD_MODEL_RESULT_FILE_NOT_FOUND = 1,
  LOAD_MODEL_FAILED_TO_READ_FILE = 2,
  LOAD_MODEL_RESULT_INVALID_FORMAT = 3,
  LOAD_MODEL_RESULT_NOT_TRIANGULATED = 4,
  LOAD_MODEL_RESULT_OUT_OF_MEMORY = 5,
  LOAD_MODEL_RESULT_NO_GEOMETRY_DATA = 6,
  LOAD_MODEL_UNKNOWN_ERROR = 7,
  LOAD_MODEL_RESULT_INVALID_TARGET = 8
};

enum LoadModelResult loadModel(const char* FilePath, struct Model* Destination);
void destroyModel(const struct Model* Model);

#endif // SOFTWARE_MODEL_VISUALIZER_MODEL_H
