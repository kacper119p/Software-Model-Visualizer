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
#ifndef SOFTWARE_MODEL_VISUALIZER_LIGHTBUFFER_H
#define SOFTWARE_MODEL_VISUALIZER_LIGHTBUFFER_H

#include <stdint.h>

#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"
#include "lights/Spotlight.h"

enum LightType {
  LIGHT_TYPE_DIRECTIONAL = 0,
  LIGHT_TYPE_POINT = 1,
  LIGHT_TYPE_SPOTLIGHT = 2,
};

struct LightEntry {
  enum LightType Type;
  const void* Data;
};

struct LightBuffer {
  uint32_t* Data;
  uint32_t* Offsets;
  uint32_t DataCapacity;
  uint32_t OffsetCapacity;
  uint32_t DataSize;
  uint32_t ObjectCount;
};

/* Initializes an empty LightBuffer. Must be destroyed with LightBuffer_Destroy.
 */
void lightBufferInit(struct LightBuffer* Buffer);

void lightBufferDestroy(struct LightBuffer* Buffer);

void lightBufferClear(struct LightBuffer* Buffer);

void lightBufferRemove(struct LightBuffer* Buffer, uint32_t Index);

void lightBufferAddDirectional(struct LightBuffer* Buffer,
                               const struct DirectionalLight* Light);

void lightBufferAddPoint(struct LightBuffer* Buffer,
                         const struct PointLight* Light);

void lightBufferAddSpotlight(struct LightBuffer* Buffer,
                             const struct Spotlight* Light);

struct LightEntry lightBufferGet(const struct LightBuffer* Buffer,
                                 uint32_t Index);

static inline uint32_t lightBufferGetCount(const struct LightBuffer* Buffer) {
  return Buffer->ObjectCount;
}

static inline bool lightBufferIsEmpty(const struct LightBuffer* Buffer) {
  return Buffer->ObjectCount == 0;
}

#endif // SOFTWARE_MODEL_VISUALIZER_LIGHTBUFFER_H
