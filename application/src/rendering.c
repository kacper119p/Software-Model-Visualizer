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
#include "rendering.h"

#include "texture.h"
#include <stdlib.h>

struct ClippedVertex {
  struct Vec4 Clip;
  struct Vec3 WorldPos;
  struct Vec3 WorldNormal;
  struct Vec3 BaseLinear;
  struct Vec2 Uv;
};

static inline float edgeFunction(const struct Vec2 A, const struct Vec2 B,
                                 const struct Vec2 C) {
  return (A.X - B.X) * (C.Y - A.Y) - (A.Y - B.Y) * (C.X - A.X);
}

static inline void ndcToScreenNormalized(const struct Vec3 Ndc,
                                         struct Vec3* Normalized) {
  Normalized->X = (Ndc.X + 1.0f) * 0.5f;
  Normalized->Y = (1.0f - Ndc.Y) * 0.5f;
  Normalized->Z = (Ndc.Z + 1.0f) * 0.5f;
}

static inline struct Vec3 colorUintToLinear(const uint32_t Color) {
  const struct Color c = colorUintToColorStruct(Color);
  return MAKE_VEC3(c.R / 255.0f, c.G / 255.0f, c.B / 255.0f);
}

static inline struct ClippedVertex
lerpClippedVertex(const struct ClippedVertex A, const struct ClippedVertex B,
                  const float T) {
  struct ClippedVertex r;
  r.Clip.X = A.Clip.X + (B.Clip.X - A.Clip.X) * T;
  r.Clip.Y = A.Clip.Y + (B.Clip.Y - A.Clip.Y) * T;
  r.Clip.Z = A.Clip.Z + (B.Clip.Z - A.Clip.Z) * T;
  r.Clip.W = A.Clip.W + (B.Clip.W - A.Clip.W) * T;
  r.WorldPos = vec3Lerp(A.WorldPos, B.WorldPos, T);
  r.WorldNormal = vec3Lerp(A.WorldNormal, B.WorldNormal, T);
  r.BaseLinear = vec3Lerp(A.BaseLinear, B.BaseLinear, T);
  r.Uv = vec2Lerp(A.Uv, B.Uv, T);
  return r;
}

static inline struct Vec3 evaluateLights(const struct LightBuffer* const Lights,
                                         const struct Vec3 WorldPos,
                                         const struct Vec3 WorldNormal,
                                         const struct Vec3 BaseColor,
                                         const struct Vec3 CameraPosition) {
  struct Vec3 accumulated = VEC3_ZERO;
  if (Lights == nullptr) {
    return accumulated;
  }

  static constexpr float smoothness = 512.0f;

  const struct Vec3 viewDir = vec3Normalize(vec3Sub(CameraPosition, WorldPos));
  const uint32_t count = lightBufferGetCount(Lights);
  for (uint32_t i = 0; i < count; ++i) {
    const struct LightEntry entry = lightBufferGet(Lights, i);
    struct Vec3 lightColor = VEC3_ZERO;
    float diffuseIntensity = 0.0f;
    float specularIntensity = 0.0f;

    switch (entry.Type) {
    case LIGHT_TYPE_DIRECTIONAL: {
      const struct DirectionalLight* l = entry.Data;
      const struct Vec3 lightDir =
          vec3Normalize(vec3Scale(l->direction, -1.0f));
      diffuseIntensity = fmaxf(0.0f, vec3Dot(WorldNormal, lightDir));
      const struct Vec3 halfVec = vec3Normalize(vec3Add(lightDir, viewDir));
      specularIntensity =
          powf(fmaxf(0.0f, vec3Dot(WorldNormal, halfVec)), smoothness);
      lightColor = l->color;
      break;
    }
    case LIGHT_TYPE_POINT: {
      const struct PointLight* l = entry.Data;
      const struct Vec3 toLight = vec3Sub(l->Position, WorldPos);
      const float dist = vec3Len(toLight);
      if (dist < l->Range) {
        const struct Vec3 lightDir = vec3Div(toLight, dist);
        diffuseIntensity = fmaxf(0.0f, vec3Dot(WorldNormal, lightDir));
        const struct Vec3 halfVec = vec3Normalize(vec3Add(lightDir, viewDir));
        specularIntensity =
            powf(fmaxf(0.0f, vec3Dot(WorldNormal, halfVec)), smoothness);
        const float attenuation = 1.0f / (1.0f + l->LinearFalloff * dist +
                                          l->QuadraticFalloff * dist * dist);
        diffuseIntensity *= attenuation;
        specularIntensity *= attenuation;
        lightColor = l->Color;
      }
      break;
    }
    case LIGHT_TYPE_SPOTLIGHT: {
      const struct Spotlight* l = entry.Data;
      const struct Vec3 toLight = vec3Sub(l->Position, WorldPos);
      const float dist = vec3Len(toLight);
      if (dist < l->Range) {
        const struct Vec3 lightDir = vec3Div(toLight, dist);
        diffuseIntensity = fmaxf(0.0f, vec3Dot(WorldNormal, lightDir));
        const struct Vec3 halfVec = vec3Normalize(vec3Add(lightDir, viewDir));
        specularIntensity =
            powf(fmaxf(0.0f, vec3Dot(WorldNormal, halfVec)), smoothness);
        const float cosAngle =
            vec3Dot(lightDir, vec3Normalize(vec3Scale(l->Direction, -1.0f)));
        const float cosOuter = cosf(l->OuterAngle);
        const float cosInner = cosf(l->InnerAngle);
        const float spot = fmaxf(
            0.0f, fminf(1.0f, (cosAngle - cosOuter) / (cosInner - cosOuter)));
        const float attenuation = spot / (1.0f + l->LinearFalloff * dist +
                                          l->QuadraticFalloff * dist * dist);
        diffuseIntensity *= attenuation;
        specularIntensity *= attenuation;
        lightColor = l->Color;
      }
      break;
    }
    }

    accumulated = vec3Add(
        accumulated,
        vec3Mul(lightColor,
                vec3Scale(BaseColor, diffuseIntensity + specularIntensity)));
  }

  return accumulated;
}

void clearColorBuffer(const struct Framebuffer* const Framebuffer,
                      const uint32_t ClearValue) {
  uint32_t* pixel = Framebuffer->ColorBuffer;
  const uint32_t* const end =
      Framebuffer->ColorBuffer + Framebuffer->Width * Framebuffer->Height;

  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void clearDepthBuffer(const struct Framebuffer* const Framebuffer,
                      const float ClearValue) {
  float* pixel = Framebuffer->DepthBuffer;
  const float* const end =
      Framebuffer->DepthBuffer + Framebuffer->Width * Framebuffer->Height;
  while (pixel < end) {
    *pixel = ClearValue;
    ++pixel;
  }
}

void drawPixel(const struct Framebuffer* const Framebuffer, const uint32_t X,
               const uint32_t Y, const uint32_t Color, const float Depth) {
  if (X >= Framebuffer->Width || Y >= Framebuffer->Height) {
    return;
  }
  const size_t pixelIndex = Y * Framebuffer->Width + X;
  Framebuffer->ColorBuffer[pixelIndex] = Color;
  Framebuffer->DepthBuffer[pixelIndex] = Depth;
}

void drawLine(const struct Framebuffer* const Framebuffer, const struct Vec3 V0,
              const struct Vec3 V1, const uint32_t Color) {
  struct Vec3 norm0;
  struct Vec3 norm1;
  ndcToScreenNormalized(V0, &norm0);
  ndcToScreenNormalized(V1, &norm1);

  int32_t x0 = (int32_t)(norm0.X * Framebuffer->Width);
  int32_t y0 = (int32_t)(norm0.Y * Framebuffer->Height);
  const int32_t x1 = (int32_t)(norm1.X * Framebuffer->Width);
  const int32_t y1 = (int32_t)(norm1.Y * Framebuffer->Height);

  const int32_t dx = abs(x1 - x0);
  const int32_t dy = -abs(y1 - y0);
  const int32_t sx = x0 < x1 ? 1 : -1;
  const int32_t sy = y0 < y1 ? 1 : -1;
  int32_t err = dx + dy;

  const int32_t steps = dx > -dy ? dx : -dy;
  const float depthStep = steps > 0 ? (norm1.Z - norm0.Z) / (float)steps : 0.0f;
  float depth = norm0.Z;

  while (true) {
    if ((uint32_t)x0 < Framebuffer->Width &&
        (uint32_t)y0 < Framebuffer->Height) {
      const size_t pixelIndex = (size_t)y0 * Framebuffer->Width + (size_t)x0;
      if (depth < Framebuffer->DepthBuffer[pixelIndex] && depth >= -1.0f &&
          depth <= 1.0f) {
        Framebuffer->DepthBuffer[pixelIndex] = depth;
        Framebuffer->ColorBuffer[pixelIndex] = Color;
      }
    }

    if (x0 == x1 && y0 == y1) {
      break;
    }

    const int32_t e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
    depth += depthStep;
  }
}

void drawTriangle(const struct Framebuffer* Framebuffer, const struct Vec3 V0,
                  const struct Vec3 V1, const struct Vec3 V2, const float InvW0,
                  const float InvW1, const float InvW2,
                  const struct Vec3 WorldPos0, const struct Vec3 WorldPos1,
                  const struct Vec3 WorldPos2, const struct Vec3 WorldNormal0,
                  const struct Vec3 WorldNormal1,
                  const struct Vec3 WorldNormal2, const struct Vec3 BaseColor0,
                  const struct Vec3 BaseColor1, const struct Vec3 BaseColor2,
                  const struct Vec2 Uv0, const struct Vec2 Uv1,
                  const struct Vec2 Uv2, const struct Texture* Texture,
                  const struct LightBuffer* PixelLights,
                  struct Vec3 CameraPosition) {
  struct Vec3 norm0;
  struct Vec3 norm1;
  struct Vec3 norm2;
  ndcToScreenNormalized(V0, &norm0);
  ndcToScreenNormalized(V1, &norm1);
  ndcToScreenNormalized(V2, &norm2);

  const struct Vec2 pixelV0 =
      MAKE_VEC2(norm0.X * Framebuffer->Width, norm0.Y * Framebuffer->Height);
  const struct Vec2 pixelV1 =
      MAKE_VEC2(norm1.X * Framebuffer->Width, norm1.Y * Framebuffer->Height);
  const struct Vec2 pixelV2 =
      MAKE_VEC2(norm2.X * Framebuffer->Width, norm2.Y * Framebuffer->Height);

  const float area = edgeFunction(pixelV0, pixelV1, pixelV2);

  if (area <= 0.0f) {
    return;
  }

  const float areaInv = 1.0f / area;

  const float minXF = fminf(pixelV0.X, fminf(pixelV1.X, pixelV2.X));
  const float minYF = fminf(pixelV0.Y, fminf(pixelV1.Y, pixelV2.Y));
  const float maxXF = fmaxf(pixelV0.X, fmaxf(pixelV1.X, pixelV2.X));
  const float maxYF = fmaxf(pixelV0.Y, fmaxf(pixelV1.Y, pixelV2.Y));

  const int32_t minX = max(0, (int32_t)floorf(minXF));
  const int32_t minY = max(0, (int32_t)floorf(minYF));
  const int32_t maxX = min((int32_t)Framebuffer->Width, (int32_t)ceilf(maxXF));
  const int32_t maxY = min((int32_t)Framebuffer->Height, (int32_t)ceilf(maxYF));

  const float stepXW0 = pixelV2.Y - pixelV1.Y;
  const float stepYW0 = pixelV1.X - pixelV2.X;

  const float stepXW1 = pixelV0.Y - pixelV2.Y;
  const float stepYW1 = pixelV2.X - pixelV0.X;

  const float stepXW2 = pixelV1.Y - pixelV0.Y;
  const float stepYW2 = pixelV0.X - pixelV1.X;

  const bool isTopLeft0 =
      (stepXW0 > 0.0f) || (stepXW0 == 0.0f && stepYW0 < 0.0f);
  const bool isTopLeft1 =
      (stepXW1 > 0.0f) || (stepXW1 == 0.0f && stepYW1 < 0.0f);
  const bool isTopLeft2 =
      (stepXW2 > 0.0f) || (stepXW2 == 0.0f && stepYW2 < 0.0f);

  const bool hasPixelLights =
      PixelLights != nullptr && !lightBufferIsEmpty(PixelLights);

  const struct Vec3 worldPosition0 = vec3Scale(WorldPos0, InvW0);
  const struct Vec3 worldPosition1 = vec3Scale(WorldPos1, InvW1);
  const struct Vec3 worldPosition2 = vec3Scale(WorldPos2, InvW2);
  const struct Vec3 worldNormal0 =
      vec3Normalize(vec3Scale(WorldNormal0, InvW0));
  const struct Vec3 worldNormal2 =
      vec3Normalize(vec3Scale(WorldNormal1, InvW1));
  const struct Vec3 worldNormal3 =
      vec3Normalize(vec3Scale(WorldNormal2, InvW2));
  const struct Vec3 baseColor0 = vec3Scale(BaseColor0, InvW0);
  const struct Vec3 baseColor1 = vec3Scale(BaseColor1, InvW1);
  const struct Vec3 baseColor2 = vec3Scale(BaseColor2, InvW2);
  const struct Vec2 uv0 = MAKE_VEC2(Uv0.X * InvW0, Uv0.Y * InvW0);
  const struct Vec2 uv1 = MAKE_VEC2(Uv1.X * InvW1, Uv1.Y * InvW1);
  const struct Vec2 uv2 = MAKE_VEC2(Uv2.X * InvW2, Uv2.Y * InvW2);

  size_t rowStartIndex = (size_t)minY * Framebuffer->Width;

  for (int32_t y = minY; y < maxY; ++y) {
    const float pixelCenterY = (float)y + 0.5f;
    for (int32_t x = minX; x < maxX; ++x) {
      const struct Vec2 pixelCenter = MAKE_VEC2((float)x + 0.5f, pixelCenterY);
      const float w0 = edgeFunction(pixelV1, pixelV2, pixelCenter);
      const float w1 = edgeFunction(pixelV2, pixelV0, pixelCenter);
      const float w2 = edgeFunction(pixelV0, pixelV1, pixelCenter);

      const bool pass0 = (w0 > 0.0f) || (w0 == 0.0f && isTopLeft0);
      const bool pass1 = (w1 > 0.0f) || (w1 == 0.0f && isTopLeft1);
      const bool pass2 = (w2 > 0.0f) || (w2 == 0.0f && isTopLeft2);

      if (pass0 && pass1 && pass2) {
        const float b0 = w0 * areaInv;
        const float b1 = w1 * areaInv;
        const float b2 = w2 * areaInv;

        const float depth = b0 * norm0.Z + b1 * norm1.Z + b2 * norm2.Z;
        const size_t pixelIndex = rowStartIndex + (size_t)x;

        const bool notCulled = depth < Framebuffer->DepthBuffer[pixelIndex] &&
                               depth >= -1.0f && depth <= 1.0f;

        if (notCulled) {
          const float invWInterp = b0 * InvW0 + b1 * InvW1 + b2 * InvW2;
          const float wInterp = 1.0f / invWInterp;

          const struct Vec2 uv =
              MAKE_VEC2((b0 * uv0.X + b1 * uv1.X + b2 * uv2.X) * wInterp,
                        (b0 * uv0.Y + b1 * uv1.Y + b2 * uv2.Y) * wInterp);
          const struct Color texel = Texture != nullptr
                                         ? textureSample(Texture, uv)
                                         : (struct Color){0, 0, 0};
          const struct Vec3 baseLinear =
              Texture != nullptr
                  ? MAKE_VEC3(texel.R / 255.0f, texel.G / 255.0f,
                              texel.B / 255.0f)
                  : vec3Scale(MAKE_VEC3(b0 * baseColor0.X + b1 * baseColor1.X +
                                            b2 * baseColor2.X,
                                        b0 * baseColor0.Y + b1 * baseColor1.Y +
                                            b2 * baseColor2.Y,
                                        b0 * baseColor0.Z + b1 * baseColor1.Z +
                                            b2 * baseColor2.Z),
                              wInterp);

          struct Vec3 final = baseLinear;

          if (hasPixelLights) {
            const struct Vec3 worldPos = vec3Scale(
                MAKE_VEC3(b0 * worldPosition0.X + b1 * worldPosition1.X +
                              b2 * worldPosition2.X,
                          b0 * worldPosition0.Y + b1 * worldPosition1.Y +
                              b2 * worldPosition2.Y,
                          b0 * worldPosition0.Z + b1 * worldPosition1.Z +
                              b2 * worldPosition2.Z),
                wInterp);
            const struct Vec3 worldNormal = vec3Normalize(
                vec3Scale(MAKE_VEC3(b0 * worldNormal0.X + b1 * worldNormal2.X +
                                        b2 * worldNormal3.X,
                                    b0 * worldNormal0.Y + b1 * worldNormal2.Y +
                                        b2 * worldNormal3.Y,
                                    b0 * worldNormal0.Z + b1 * worldNormal2.Z +
                                        b2 * worldNormal3.Z),
                          wInterp));

            final = evaluateLights(PixelLights, worldPos, worldNormal,
                                   baseLinear, CameraPosition);
          }

          Framebuffer->DepthBuffer[pixelIndex] = depth;
          const struct Color outColor = {
              (uint8_t)(fminf(final.X, 1.0f) * 255.0f),
              (uint8_t)(fminf(final.Y, 1.0f) * 255.0f),
              (uint8_t)(fminf(final.Z, 1.0f) * 255.0f)};
          Framebuffer->ColorBuffer[pixelIndex] =
              colorStructToColorUint(outColor);
        }
      }
    }

    rowStartIndex += Framebuffer->Width;
  }
}

static int32_t clipTriangleNearPlane(const struct ClippedVertex In[3],
                                     struct ClippedVertex Out[4]) {
  int32_t outCount = 0;
  for (int32_t i = 0; i < 3; ++i) {
    const struct ClippedVertex curr = In[i];
    const struct ClippedVertex next = In[(i + 1) % 3];
    const float dCurr = curr.Clip.Z + curr.Clip.W;
    const float dNext = next.Clip.Z + next.Clip.W;
    const bool currInside = dCurr >= 0.0f;
    const bool nextInside = dNext >= 0.0f;
    if (currInside) {
      Out[outCount++] = curr;
      if (!nextInside) {
        const float t = dCurr / (dCurr - dNext);
        Out[outCount++] = lerpClippedVertex(curr, next, t);
      }
    } else if (nextInside) {
      const float t = dCurr / (dCurr - dNext);
      Out[outCount++] = lerpClippedVertex(curr, next, t);
    }
  }
  return outCount;
}

void drawModel(const struct Framebuffer* Framebuffer, const struct Model* Model,
               const struct Mat4 ModelMatrix, const struct Mat4 MvpMatrix,
               const struct Texture* Texture, const bool Lit,
               const struct LightBuffer* const PixelLights,
               const struct Vec3 CameraPosition) {
  for (size_t i = 0; i < Model->IndexCount; i += 3) {
    const uint32_t index0 = Model->Indices[i];
    const uint32_t index1 = Model->Indices[i + 1];
    const uint32_t index2 = Model->Indices[i + 2];

    const struct Vec4 world0 =
        mat4MulVec4(ModelMatrix, MAKE_VEC4(Model->Vertices[index0].X,
                                           Model->Vertices[index0].Y,
                                           Model->Vertices[index0].Z, 1.0f));
    const struct Vec4 world1 =
        mat4MulVec4(ModelMatrix, MAKE_VEC4(Model->Vertices[index1].X,
                                           Model->Vertices[index1].Y,
                                           Model->Vertices[index1].Z, 1.0f));
    const struct Vec4 world2 =
        mat4MulVec4(ModelMatrix, MAKE_VEC4(Model->Vertices[index2].X,
                                           Model->Vertices[index2].Y,
                                           Model->Vertices[index2].Z, 1.0f));

    const struct Vec4 worldNormal0Raw =
        mat4MulVec4(ModelMatrix, MAKE_VEC4(Model->Normals[index0].X,
                                           Model->Normals[index0].Y,
                                           Model->Normals[index0].Z, 0.0f));
    const struct Vec4 worldNormal1Raw =
        mat4MulVec4(ModelMatrix, MAKE_VEC4(Model->Normals[index1].X,
                                           Model->Normals[index1].Y,
                                           Model->Normals[index1].Z, 0.0f));
    const struct Vec4 worldNormal2Raw =
        mat4MulVec4(ModelMatrix, MAKE_VEC4(Model->Normals[index2].X,
                                           Model->Normals[index2].Y,
                                           Model->Normals[index2].Z, 0.0f));

    const struct Vec3 worldPos0 = MAKE_VEC3(world0.X, world0.Y, world0.Z);
    const struct Vec3 worldPos1 = MAKE_VEC3(world1.X, world1.Y, world1.Z);
    const struct Vec3 worldPos2 = MAKE_VEC3(world2.X, world2.Y, world2.Z);
    const struct Vec3 worldNormal0 = vec3Normalize(
        MAKE_VEC3(worldNormal0Raw.X, worldNormal0Raw.Y, worldNormal0Raw.Z));
    const struct Vec3 worldNormal1 = vec3Normalize(
        MAKE_VEC3(worldNormal1Raw.X, worldNormal1Raw.Y, worldNormal1Raw.Z));
    const struct Vec3 worldNormal2 = vec3Normalize(
        MAKE_VEC3(worldNormal2Raw.X, worldNormal2Raw.Y, worldNormal2Raw.Z));

    const struct Vec3 baseLinear = colorUintToLinear(Model->Colors[i / 3]);

    const struct Vec2 uv0 =
        Model->TextureCoords ? Model->TextureCoords[index0] : VEC2_ZERO;
    const struct Vec2 uv1 =
        Model->TextureCoords ? Model->TextureCoords[index1] : VEC2_ZERO;
    const struct Vec2 uv2 =
        Model->TextureCoords ? Model->TextureCoords[index2] : VEC2_ZERO;

    const struct Vec4 clip0 =
        mat4MulVec4(MvpMatrix, MAKE_VEC4(Model->Vertices[index0].X,
                                         Model->Vertices[index0].Y,
                                         Model->Vertices[index0].Z, 1.0f));
    const struct Vec4 clip1 =
        mat4MulVec4(MvpMatrix, MAKE_VEC4(Model->Vertices[index1].X,
                                         Model->Vertices[index1].Y,
                                         Model->Vertices[index1].Z, 1.0f));
    const struct Vec4 clip2 =
        mat4MulVec4(MvpMatrix, MAKE_VEC4(Model->Vertices[index2].X,
                                         Model->Vertices[index2].Y,
                                         Model->Vertices[index2].Z, 1.0f));

    const struct ClippedVertex in[3] = {
        {clip0, worldPos0, worldNormal0, baseLinear, uv0},
        {clip1, worldPos1, worldNormal1, baseLinear, uv1},
        {clip2, worldPos2, worldNormal2, baseLinear, uv2}};
    struct ClippedVertex out[4];
    const int outCount = clipTriangleNearPlane(in, out);
    if (outCount < 3) {
      continue;
    }

    struct Vec3 ndc[4];
    float invW[4];
    for (int k = 0; k < outCount; ++k) {
      invW[k] = 1.0f / out[k].Clip.W;
      ndc[k] = MAKE_VEC3(out[k].Clip.X * invW[k], out[k].Clip.Y * invW[k],
                         out[k].Clip.Z * invW[k]);
    }

    for (int k = 1; k <= outCount - 2; ++k) {
      drawTriangle(Framebuffer, ndc[0], ndc[k], ndc[k + 1], invW[0], invW[k],
                   invW[k + 1], out[0].WorldPos, out[k].WorldPos,
                   out[k + 1].WorldPos, out[0].WorldNormal, out[k].WorldNormal,
                   out[k + 1].WorldNormal, out[0].BaseLinear, out[k].BaseLinear,
                   out[k + 1].BaseLinear, out[0].Uv, out[k].Uv, out[k + 1].Uv,
                   Texture, Lit ? PixelLights : nullptr, CameraPosition);
    }
  }
}

void drawModelMesh(const struct Framebuffer* Framebuffer,
                   const struct Model* Model, const struct Mat4 Transform,
                   const uint32_t Color) {
  for (size_t i = 0; i < Model->IndexCount; i += 3) {
    const uint32_t index0 = Model->Indices[i];
    const uint32_t index1 = Model->Indices[i + 1];
    const uint32_t index2 = Model->Indices[i + 2];
    const struct Vec4 obj0 =
        MAKE_VEC4(Model->Vertices[index0].X, Model->Vertices[index0].Y,
                  Model->Vertices[index0].Z, 1.0f);
    const struct Vec4 obj1 =
        MAKE_VEC4(Model->Vertices[index1].X, Model->Vertices[index1].Y,
                  Model->Vertices[index1].Z, 1.0f);
    const struct Vec4 obj2 =
        MAKE_VEC4(Model->Vertices[index2].X, Model->Vertices[index2].Y,
                  Model->Vertices[index2].Z, 1.0f);

    const struct Vec4 clip0 = mat4MulVec4(Transform, obj0);
    const struct Vec4 clip1 = mat4MulVec4(Transform, obj1);
    const struct Vec4 clip2 = mat4MulVec4(Transform, obj2);

    const struct Vec3 ndc0 =
        MAKE_VEC3(clip0.X / clip0.W, clip0.Y / clip0.W, clip0.Z / clip0.W);
    const struct Vec3 ndc1 =
        MAKE_VEC3(clip1.X / clip1.W, clip1.Y / clip1.W, clip1.Z / clip1.W);
    const struct Vec3 ndc2 =
        MAKE_VEC3(clip2.X / clip2.W, clip2.Y / clip2.W, clip2.Z / clip2.W);

    drawLine(Framebuffer, ndc0, ndc1, Color);
    drawLine(Framebuffer, ndc1, ndc2, Color);
    drawLine(Framebuffer, ndc2, ndc0, Color);
  }
}
