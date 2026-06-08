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
#ifndef SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#define SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#include "appWindow.h"
#include "graphicsMath.h"
#include "lights/lightBuffer.h"
#include "model.h"
#include "texture.h"

void clearColorBuffer(const struct Framebuffer* Framebuffer,
                      uint32_t ClearValue);
void clearDepthBuffer(const struct Framebuffer* Framebuffer, float ClearValue);
void drawPixel(const struct Framebuffer* Framebuffer, uint32_t X, uint32_t Y,
               uint32_t Color, float Depth);
void drawLine(const struct Framebuffer* Framebuffer, struct Vec3 V0,
              struct Vec3 V1, uint32_t Color);
void drawTriangle(const struct Framebuffer* Framebuffer, struct Vec3 V0,
                  struct Vec3 V1, struct Vec3 V2, float InvW0, float InvW1,
                  float InvW2, struct Vec3 WorldPos0, struct Vec3 WorldPos1,
                  struct Vec3 WorldPos2, struct Vec3 WorldNormal0,
                  struct Vec3 WorldNormal1, struct Vec3 WorldNormal2,
                  struct Vec3 BaseColor0, struct Vec3 BaseColor1,
                  struct Vec3 BaseColor2, struct Vec2 Uv0, struct Vec2 Uv1,
                  struct Vec2 Uv2, const struct Texture* Texture,
                  const struct LightBuffer* PixelLights,
                  struct Vec3 CameraPosition);
void drawModel(const struct Framebuffer* Framebuffer, const struct Model* Model,
               struct Mat4 ModelMatrix, struct Mat4 MvpMatrix,
               const struct Texture* Texture,
               const struct LightBuffer* PixelLights,
               struct Vec3 CameraPosition);
void drawModelMesh(const struct Framebuffer* Framebuffer,
                   const struct Model* Model, struct Mat4 Transform,
                   uint32_t Color);

#endif // SOFTWARE_MODEL_VISUALIZER_RENDERING_H
