#include "model.h"

#include <time.h>

#include "cglm/affine.h"
#include "cglm/cam.h"
#include "rendering.h"
#include "timeQuery.h"
#include "window.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
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
  LoadModel(argv[1], &model, modelCenter, &extent);

  srand(time(nullptr));

  Window* window = createWindow();

  TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  while (!window->ShouldClose) {
    const float currentTime = getElapsedTime(&timeQuery);
    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00000000);
    clearDepthBuffer(&window->Framebuffer, 1.0f);
    mat4 projectionMatrix;
    constexpr float cameraDistanceMultiplier = 1.2f;
    const float aspect =
        (float)window->Framebuffer.Width / (float)window->Framebuffer.Height;
    glm_perspective(glm_rad(70.0f), aspect, 0.1f,
                    extent * 2.0f * cameraDistanceMultiplier, projectionMatrix);
    mat4 viewMatrix;
    vec3 eye = {0.0f, 0.0f, extent * cameraDistanceMultiplier};
    vec3 sceneCenter = {0.0f, 0.0f, 0.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_lookat(eye, sceneCenter, up, viewMatrix);

    mat4 modelMatrix;
    vec3 translation;
    glm_vec3_scale(modelCenter, -1.0f, translation);
    glm_translate_make(modelMatrix, translation);
    glm_rotate(modelMatrix, currentTime, (vec3){0.0f, 1.0f, 0.0f});

    mat4 vpMatrix;
    glm_mat4_mul(projectionMatrix, viewMatrix, vpMatrix);
    mat4 mvpMatrix;
    glm_mat4_mul(vpMatrix, modelMatrix, mvpMatrix);

    drawModel(&window->Framebuffer, &model, mvpMatrix);
    presentWindow(window);
  }

  DestroyModel(&model);
  destroyWindow(window);
  return 0;
}
