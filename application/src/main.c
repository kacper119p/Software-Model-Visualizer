#include "model.h"

#include <time.h>

#include "cglm/affine.h"
#include "cglm/mat4.h"
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
  float extent;
  LoadModel(argv[1], &model, &extent);

  srand(time(nullptr));

  Window* window = createWindow();

  TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  while (!window->ShouldClose) {
    const float currentTime = getElapsedTime(&timeQuery);
    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00000000);
    clearDepthBuffer(&window->Framebuffer, 1.0f);
    mat4 transform;
    vec3 translation = {0.5f, 0.75f, 0.0f};
    glm_translate_make(transform, translation);
    vec3 scale = {0.18f, -0.18f, 0.18f};
    glm_scale(transform, scale);
    glm_rotate(transform, currentTime, (vec3){0.0f, 1.0f, 0.0f});
    drawModel(&window->Framebuffer, &model, transform);
    presentWindow(window);
  }

  DestroyModel(&model);
  destroyWindow(window);
  return 0;
}
