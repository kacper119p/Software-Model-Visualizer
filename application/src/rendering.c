#include "rendering.h"

static float edgeFunction(const ivec2 A, const ivec2 B, const ivec2 C) {
  return (A[0] - B[0]) * (C[1] - A[1]) - (A[1] - B[1]) * (C[0] - A[0]);
}

void clearColorBuffer(const Framebuffer* const Framebuffer,
                      const uint32_t ClearValue) {
  uint32_t* pixel = Framebuffer->ColorBuffer;
  const uint32_t* const end =
      Framebuffer->ColorBuffer + Framebuffer->Width * Framebuffer->Height;

  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void clearDepthBuffer(const Framebuffer* const Framebuffer,
                      const float ClearValue) {
  float* pixel = Framebuffer->DepthBuffer;
  const float* const end =
      Framebuffer->DepthBuffer + Framebuffer->Width * Framebuffer->Height;
  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void drawPixel(const Framebuffer* const Framebuffer, const uint32_t X,
               const uint32_t Y, const uint32_t Color, const float Depth) {
  if (X >= Framebuffer->Width || Y >= Framebuffer->Height) {
    return;
  }
  const size_t pixelIndex = Y * Framebuffer->Width + X;
  Framebuffer->ColorBuffer[pixelIndex] = Color;
  Framebuffer->DepthBuffer[pixelIndex] = Depth;
}

void drawTriangle(const Framebuffer* Framebuffer, vec3 V0, vec3 V1, vec3 V2,
                  const uint32_t Color) {
  const ivec2 pixelV0 = {(int32_t)(V0[0] * Framebuffer->Width),
                         (int32_t)(V0[1] * Framebuffer->Height)};
  const ivec2 pixelV1 = {(int32_t)(V1[0] * Framebuffer->Width),
                         (int32_t)(V1[1] * Framebuffer->Height)};
  const ivec2 pixelV2 = {(int32_t)(V2[0] * Framebuffer->Width),
                         (int32_t)(V2[1] * Framebuffer->Height)};

  const float area = edgeFunction(pixelV0, pixelV1, pixelV2);

  if (area <= 0.0f) {
    return;
  }

  const int32_t minX = max(0, min(pixelV0[0], min(pixelV1[0], pixelV2[0])));
  const int32_t minY = max(0, min(pixelV0[1], min(pixelV1[1], pixelV2[1])));
  const int32_t maxX = min((int32_t)Framebuffer->Width,
                           max(pixelV0[0], max(pixelV1[0], pixelV2[0])));
  const int32_t maxY = min((int32_t)Framebuffer->Height,
                           max(pixelV0[1], max(pixelV1[1], pixelV2[1])));

  for (int32_t y = minY; y < maxY; ++y) {
    for (int32_t x = minX; x < maxX; ++x) {
      const ivec2 pixel = {x, y};
      float w0 = edgeFunction(pixelV1, pixelV2, pixel);
      if (w0 < 0.0f) {
        continue;
      }
      float w1 = edgeFunction(pixelV2, pixelV0, pixel);
      if (w1 < 0.0f) {
        continue;
      }
      float w2 = edgeFunction(pixelV0, pixelV1, pixel);
      if (w2 < 0.0f) {
        continue;
      }

      w0 /= area;
      w1 /= area;
      w2 /= area;

      const float depth = w0 * V0[2] + w1 * V1[2] + w2 * V2[2];

      const size_t pixelIndex = y * Framebuffer->Width + x;

      if (depth < Framebuffer->DepthBuffer[pixelIndex]) {
        Framebuffer->DepthBuffer[pixelIndex] = depth;
        Framebuffer->ColorBuffer[pixelIndex] = Color;
      }
    }
  }
}
