#include "model.h"

#include <time.h>

#include "appWindow.h"
#include "rendering.h"
#include "timeQuery.h"

#include <stdio.h>
#include <stdlib.h>

static inline void printUsage(const char* ProgramName) {
  fprintf(stderr,
          "Error: Invalid number of arguments.\n"
          "Usage: %s <file_path>\n",
          (ProgramName != nullptr) ? ProgramName : "rasterizer");
}

static inline int processArguments(const int Argc, const char* const Argv[],
                                   struct Model* Model) {
  if (Argc != 2) {
    printUsage(Argc > 0 ? Argv[0] : nullptr);
    return EXIT_FAILURE;
  }
  loadModel(Argv[1], Model);

  return EXIT_SUCCESS;
}

static inline struct Vec3
calculateModelCenter(const struct Model* const Model) {
  struct Vec3 center;
  center.X = (Model->AabbMin.X + Model->AabbMax.X) * 0.5f;
  center.Y = (Model->AabbMin.Y + Model->AabbMax.Y) * 0.5f;
  center.Z = (Model->AabbMin.Z + Model->AabbMax.Z) * 0.5f;
  return center;
}

static inline float calculateModelExtent(const struct Model* const Model) {
  return vec3Dist(Model->AabbMin, Model->AabbMax) / 2.0f;
}

static inline void renderFrame(struct Framebuffer* const Framebuffer,
                               const struct Model* const Model,
                               const float Time, const float Extent,
                               const struct Vec3 ModelCenter) {
  clearColorBuffer(Framebuffer, 0x00000000);
  clearDepthBuffer(Framebuffer, 1.0f);

  constexpr float cameraDistanceMultiplier = 1.2f;
  const float aspect = (float)Framebuffer->Width / (float)Framebuffer->Height;
  const struct Mat4 projectionMatrix = makePerspectiveProjectionMatrix(
      70.0f * DegToRad, aspect, 0.1f, Extent * 2.0f * cameraDistanceMultiplier);
  const struct Vec3 eye =
      MAKE_VEC3(0.0f, 0.0f, Extent * cameraDistanceMultiplier);
  const struct Vec3 sceneCenter = MAKE_VEC3(0.0f, 0.0f, 0.0f);
  const struct Vec3 up = MAKE_VEC3(0.0f, 1.0f, 0.0f);
  const struct Mat4 viewMatrix = makeMat4LookAt(eye, sceneCenter, up);

  const struct Vec3 translation = vec3Scale(ModelCenter, -1.0f);
  struct Mat4 modelMatrix = makeMat4Translation(translation);
  modelMatrix = mulMat4RotationY(modelMatrix, Time);

  const struct Mat4 mvpMatrix =
      mat4Mul(mat4Mul(projectionMatrix, viewMatrix), modelMatrix);

  drawModel(Framebuffer, Model, mvpMatrix);
}

int main(const int argc, const char* const argv[]) {
  srand(time(nullptr));

  struct Model model;
  processArguments(argc, argv, &model);

  struct AppWindow* window = createWindow();

  const struct Vec3 modelCenter = calculateModelCenter(&model);
  const float extent = calculateModelExtent(&model);

  struct TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  while (!window->ShouldClose) {
    const float currentTime = getElapsedTime(&timeQuery);
    peekWindowMessages(window);
    renderFrame(&window->Framebuffer, &model, currentTime, extent, modelCenter);
    presentWindow(window);
  }

  destroyModel(&model);
  destroyWindow(&window);
  return EXIT_SUCCESS;
}
