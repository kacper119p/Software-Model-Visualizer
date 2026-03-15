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

  uint32_t* ColorPixel =
      (uint32_t*)Framebuffer->ColorBuffer + (Y * Framebuffer->Width) + X;
  *ColorPixel = Color;

  float* DepthPixel =
      (float*)Framebuffer->DepthBuffer + (Y * Framebuffer->Width) + X;
  *DepthPixel = Depth;
}

void drawTriangle(const Framebuffer* Framebuffer, vec3 V0, vec3 V1, vec3 V2,
                  const uint32_t Color) {
  const ivec2 PixelV0 = {(int32_t)(V0[0] * Framebuffer->Width),
                         (int32_t)(V0[1] * Framebuffer->Height)};
  const ivec2 PixelV1 = {(int32_t)(V1[0] * Framebuffer->Width),
                         (int32_t)(V1[1] * Framebuffer->Height)};
  const ivec2 PixelV2 = {(int32_t)(V2[0] * Framebuffer->Width),
                         (int32_t)(V2[1] * Framebuffer->Height)};

  const float Area = edgeFunction(PixelV0, PixelV1, PixelV2);

  if (Area <= 0.0f) {
    return;
  }

  const int32_t MinX = max(0, min(PixelV0[0], min(PixelV1[0], PixelV2[0])));
  const int32_t MinY = max(0, min(PixelV0[1], min(PixelV1[1], PixelV2[1])));
  const int32_t MaxX = min((int32_t)Framebuffer->Width,
                           max(PixelV0[0], max(PixelV1[0], PixelV2[0])));
  const int32_t MaxY = min((int32_t)Framebuffer->Height,
                           max(PixelV0[1], max(PixelV1[1], PixelV2[1])));

  for (int32_t Y = MinY; Y < MaxY; ++Y) {
    for (int32_t X = MinX; X < MaxX; ++X) {
      const ivec2 Pixel = {X, Y};
      float W0 = edgeFunction(PixelV1, PixelV2, Pixel);
      float W1 = edgeFunction(PixelV2, PixelV0, Pixel);
      float W2 = edgeFunction(PixelV0, PixelV1, Pixel);

      if (W0 >= 0.0f && W1 >= 0.0f && W2 >= 0.0f) {
        W0 /= Area;
        W1 /= Area;
        W2 /= Area;

        const float Depth = W0 * V0[2] + W1 * V1[2] + W2 * V2[2];

        const size_t PixelIndex = Y * Framebuffer->Width + X;

        if (Depth < Framebuffer->DepthBuffer[PixelIndex]) {
          Framebuffer->DepthBuffer[PixelIndex] = Depth;
          Framebuffer->ColorBuffer[PixelIndex] = Color;
        }
      }
    }
  }
}
