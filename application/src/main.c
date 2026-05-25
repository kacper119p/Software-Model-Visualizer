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
#include <time.h>

#include "appWindow.h"
#include "model.h"
#include "rendering.h"
#include "timeQuery.h"

#include <stdlib.h>

static constexpr float cameraDistance = 8.0f;
static constexpr float cameraHeight = 4.0f;
static constexpr float cameraOrbitSpeed = 0.3f;

static constexpr size_t knotRingCount = 8;
static constexpr float knotRingRadius = 9.0f;

static const uint32_t knotRingParams[knotRingCount][2] = {
    {2, 3}, {3, 4}, {2, 5}, {4, 5}, {2, 13}, {3, 7}, {4, 7}, {5, 6}};

static void setupLights(struct LightBuffer* const Lights) {
  const struct DirectionalLight dir = {.direction = {-1.0f, 0.0f, 0.0f},
                                       .color = {0.7f, 0.7f, 0.7f}};
  lightBufferAddDirectional(Lights, &dir);

  const struct PointLight pointTorus = {.Position = {0.0f, 0.0f, -3.0f},
                                        .Color = {1.0f, 0.1f, 0.1f},
                                        .Range = 2.5f,
                                        .LinearFalloff = 0.5f,
                                        .QuadraticFalloff = 0.5f};
  lightBufferAddPoint(Lights, &pointTorus);

  const struct PointLight pointKnot = {.Position = {0.0f, 0.0f, 3.0f},
                                       .Color = {0.1f, 1.0f, 0.1f},
                                       .Range = 2.5f,
                                       .LinearFalloff = 0.5f,
                                       .QuadraticFalloff = 0.5f};
  lightBufferAddPoint(Lights, &pointKnot);

  const struct Spotlight spotCone = {.Position = {3.0f, 3.5f, 0.0f},
                                     .Direction = {0.0f, -1.0f, 0.0f},
                                     .Color = {1.0f, 1.0f, 0.0f},
                                     .Range = 4.0f,
                                     .LinearFalloff = 0.2f,
                                     .QuadraticFalloff = 0.1f,
                                     .OuterAngle = 0.18f,
                                     .InnerAngle = 0.08f};
  lightBufferAddSpotlight(Lights, &spotCone);

  const struct Spotlight spotSphere = {.Position = {-3.0f, 2.5f, 0.0f},
                                       .Direction = {0.0f, -1.0f, 0.0f},
                                       .Color = {0.2f, 0.4f, 1.0f},
                                       .Range = 5.0f,
                                       .LinearFalloff = 0.1f,
                                       .QuadraticFalloff = 0.05f,
                                       .OuterAngle = 0.35f,
                                       .InnerAngle = 0.15f};
  lightBufferAddSpotlight(Lights, &spotSphere);
}

static void drawAt(struct Framebuffer* const Framebuffer,
                   const struct Model* const Model, const struct Vec3 Position,
                   const struct Mat4 VpMatrix,
                   const struct LightBuffer* const Lights) {
  const struct Mat4 modelMatrix = makeMat4Translation(Position);
  drawModel(Framebuffer, Model, modelMatrix, mat4Mul(VpMatrix, modelMatrix),
            Lights);
}

static void setWhite(struct Model* const Model) {
  for (size_t i = 0; i < Model->IndexCount / 3; ++i) {
    Model->Colors[i] = 0xFFFFFF;
  }
}

static void renderFrame(struct Framebuffer* const Framebuffer,
                        const struct Model* const Sphere,
                        const struct Model* const Cone,
                        const struct Model* const Torus,
                        const struct Model* const TorusKnot,
                        const struct Model* const KnotRing, const float Time,
                        const struct LightBuffer* const Lights) {
  clearColorBuffer(Framebuffer, 0x00000000);
  clearDepthBuffer(Framebuffer, 1.0f);

  const float aspect = (float)Framebuffer->Width / (float)Framebuffer->Height;
  const struct Mat4 proj =
      makePerspectiveProjectionMatrix(60.0f * DegToRad, aspect, 0.1f, 30.0f);
  const float angle = Time * cameraOrbitSpeed;
  const struct Vec3 eye = MAKE_VEC3(cameraDistance * sinf(angle), cameraHeight,
                                    cameraDistance * cosf(angle));
  const struct Mat4 vp = mat4Mul(
      proj, makeMat4LookAt(eye, VEC3_ZERO, MAKE_VEC3(0.0f, 1.0f, 0.0f)));

  drawAt(Framebuffer, Sphere, MAKE_VEC3(-3.0f, 0.0f, 0.0f), vp, Lights);
  drawAt(Framebuffer, Sphere, MAKE_VEC3(-3.0f, 2.5f, 0.0f), vp, Lights);
  drawAt(Framebuffer, Cone, MAKE_VEC3(3.0f, 0.0f, 0.0f), vp, Lights);
  drawAt(Framebuffer, Torus, MAKE_VEC3(0.0f, 0.0f, -3.0f), vp, Lights);
  drawAt(Framebuffer, TorusKnot, MAKE_VEC3(0.0f, 0.0f, 3.0f), vp, Lights);

  for (size_t i = 0; i < knotRingCount; ++i) {
    const float angle = (float)i * (2.0f * Pi / (float)knotRingCount);
    const struct Vec3 pos = MAKE_VEC3(knotRingRadius * cosf(angle), 0.0f,
                                      knotRingRadius * sinf(angle));
    drawAt(Framebuffer, &KnotRing[i], pos, vp, Lights);
  }
}

int main(void) {
  srand(time(nullptr));

  struct Model sphere = {0};
  struct Model cone = {0};
  struct Model torus = {0};
  struct Model torusKnot = {0};
  if (!generateSphereModel(0.8f, 64, 48, &sphere) ||
      !generateConeModel(0.7f, 1.5f, 24, &cone) ||
      !generateTorusModel(0.7f, 0.3f, 32, 16, &torus) ||
      !generateTorusKnotModel(0.6f, 0.2f, 2, 3, 128, 16, &torusKnot)) {
    return EXIT_FAILURE;
  }
  setWhite(&sphere);
  setWhite(&cone);
  setWhite(&torus);
  setWhite(&torusKnot);

  struct Model knotRing[knotRingCount] = {0};
  for (size_t i = 0; i < knotRingCount; ++i) {
    if (!generateTorusKnotModel(0.45f, 0.12f, knotRingParams[i][0],
                                knotRingParams[i][1], 96, 12, &knotRing[i])) {
      for (size_t j = 0; j < i; ++j) {
        destroyModel(&knotRing[j]);
      }
      destroyModel(&sphere);
      destroyModel(&cone);
      destroyModel(&torus);
      destroyModel(&torusKnot);
      return EXIT_FAILURE;
    }
    setWhite(&knotRing[i]);
  }

  struct AppWindow window;
  createWindow(&window);

  struct TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  struct LightBuffer lightBuffer;
  lightBufferInit(&lightBuffer);
  setupLights(&lightBuffer);

  while (!window.ShouldClose) {
    const float currentTime = getElapsedTime(&timeQuery);
    peekWindowMessages(&window);
    renderFrame(&window.Framebuffer, &sphere, &cone, &torus, &torusKnot,
                knotRing, currentTime, &lightBuffer);
    presentWindow(&window);
  }

  destroyModel(&sphere);
  destroyModel(&cone);
  destroyModel(&torus);
  destroyModel(&torusKnot);
  for (size_t i = 0; i < knotRingCount; ++i) {
    destroyModel(&knotRing[i]);
  }
  destroyWindow(&window);
  lightBufferDestroy(&lightBuffer);
  return EXIT_SUCCESS;
}
