#include "model.h"

#include <time.h>

#include "appWindow.h"
#include "rendering.h"
#include "timeQuery.h"

#include <stdio.h>
#include <stdlib.h>

int main(const int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr,
            "Error: Invalid number of arguments.\n"
            "Usage: %s <file_path>\n",
            (argc > 0) ? argv[0] : "rasterizer");
    return EXIT_FAILURE;
  }

  Model model;
  vec3 modelCenter;
  float extent;
  loadModel(argv[1], &model, &modelCenter, &extent);

  srand(time(nullptr));

  AppWindow* window = createWindow();

  TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  while (!window->ShouldClose) {
    const float currentTime = getElapsedTime(&timeQuery);

    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00000000);
    clearDepthBuffer(&window->Framebuffer, 1.0f);

    constexpr float cameraDistanceMultiplier = 1.2f;
    const float aspect =
        (float)window->Framebuffer.Width / (float)window->Framebuffer.Height;
    const mat4 projectionMatrix = makePerspectiveProjectionMatrix(
        70.0f * DegToRad, aspect, 0.1f,
        extent * 2.0f * cameraDistanceMultiplier);
    const vec3 eye = MAKE_VEC3(0.0f, 0.0f, extent * cameraDistanceMultiplier);
    const vec3 sceneCenter = MAKE_VEC3(0.0f, 0.0f, 0.0f);
    const vec3 up = MAKE_VEC3(0.0f, 1.0f, 0.0f);
    const mat4 viewMatrix = makeMat4LookAt(eye, sceneCenter, up);

    const vec3 translation = vec3Scale(modelCenter, -1.0f);
    mat4 modelMatrix = makeMat4Translation(translation);
    modelMatrix = mulMat4RotationY(modelMatrix, currentTime);

    const mat4 mvpMatrix =
        mat4Mul(mat4Mul(projectionMatrix, viewMatrix), modelMatrix);

    drawModel(&window->Framebuffer, &model, mvpMatrix);
    presentWindow(window);
  }

  destroyModel(&model);
  destroyWindow(&window);
  return EXIT_SUCCESS;
}
