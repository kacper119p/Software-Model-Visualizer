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
#include "lights/LightBuffer.h"

#include <stdlib.h>
#include <string.h>

#define WORD_SIZE ((uint32_t)sizeof(uint32_t))

static uint32_t wordsFor(uint32_t Bytes) {
  return (Bytes + WORD_SIZE - 1) / WORD_SIZE;
}

static void growData(struct LightBuffer* Buffer, uint32_t Required) {
  Buffer->DataCapacity = (Buffer->DataSize + Required) * 2;
  Buffer->Data = realloc(Buffer->Data, Buffer->DataCapacity * WORD_SIZE);
}

static void growOffsets(struct LightBuffer* Buffer) {
  Buffer->OffsetCapacity = (Buffer->ObjectCount + 1) * 2;
  Buffer->Offsets =
      realloc(Buffer->Offsets, Buffer->OffsetCapacity * WORD_SIZE);
}

static void append(struct LightBuffer* Buffer, const enum LightType Type, const void* Data,
                   const uint32_t DataBytes) {
  const uint32_t dataWords = wordsFor(DataBytes);
  const uint32_t required = 1 + dataWords;

  if (Buffer->DataSize + required > Buffer->DataCapacity) {
    growData(Buffer, required);
  }
  if (Buffer->ObjectCount + 1 > Buffer->OffsetCapacity) {
    growOffsets(Buffer);
  }

  Buffer->Offsets[Buffer->ObjectCount++] = Buffer->DataSize;
  Buffer->Data[Buffer->DataSize++] = (uint32_t)Type;
  memcpy(Buffer->Data + Buffer->DataSize, Data, DataBytes);
  Buffer->DataSize += dataWords;
}

void lightBufferInit(struct LightBuffer* Buffer) { *Buffer = (struct LightBuffer){0}; }

void lightBufferDestroy(struct LightBuffer* Buffer) {
  free(Buffer->Data);
  free(Buffer->Offsets);
  *Buffer = (struct LightBuffer){0};
}

void lightBufferClear(struct LightBuffer* Buffer) {
  Buffer->DataSize = 0;
  Buffer->ObjectCount = 0;
}

void lightBufferRemove(struct LightBuffer* Buffer, const uint32_t Index) {
  const uint32_t entryStart = Buffer->Offsets[Index];
  const uint32_t entryEnd = (Index + 1 < Buffer->ObjectCount)
                                ? Buffer->Offsets[Index + 1]
                                : Buffer->DataSize;
  const uint32_t entryWords = entryEnd - entryStart;

  memmove(Buffer->Data + entryStart, Buffer->Data + entryEnd,
          (Buffer->DataSize - entryEnd) * WORD_SIZE);
  Buffer->DataSize -= entryWords;

  for (uint32_t i = Index; i < Buffer->ObjectCount - 1; ++i) {
    Buffer->Offsets[i] = Buffer->Offsets[i + 1] - entryWords;
  }
  --Buffer->ObjectCount;
}

struct LightEntry lightBufferGet(const struct LightBuffer* Buffer, const uint32_t Index) {
  const uint32_t* entry = Buffer->Data + Buffer->Offsets[Index];
  return (struct LightEntry){.Type = (enum  LightType)entry[0], .Data = entry + 1};
}

void lightBufferAddDirectional(struct LightBuffer* Buffer,
                               const struct DirectionalLight* Light) {
  append(Buffer, LIGHT_TYPE_DIRECTIONAL, Light, sizeof(*Light));
}

void lightBufferAddPoint(struct LightBuffer* Buffer, const struct PointLight* Light) {
  append(Buffer, LIGHT_TYPE_POINT, Light, sizeof(*Light));
}

void lightBufferAddSpotlight(struct LightBuffer* Buffer,
                             const struct Spotlight* Light) {
  append(Buffer, LIGHT_TYPE_SPOTLIGHT, Light, sizeof(*Light));
}
