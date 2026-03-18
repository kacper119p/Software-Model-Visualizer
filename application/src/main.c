#include "model.h"

#include <time.h>

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
  LoadModel(argv[1], &model);

  srand(time(nullptr));

  Window* window = createWindow();

  TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  // float* times = calloc(1000, sizeof(float));

  // float previousTime = getElapsedTime(&timeQuery);

  // size_t frameCount = 0;
  while (!window->ShouldClose) {
    // const float currentTime = getElapsedTime(&timeQuery);
    // const float deltaTime = currentTime - previousTime;
    // previousTime = currentTime;
    // times[frameCount++] = deltaTime;
    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00FF0070);
    clearDepthBuffer(&window->Framebuffer, 1.0f);
    drawTriangle(&window->Framebuffer, (vec3){0.5f, 0.0f, 0.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, (vec3){1.0f, 1.0f, 0.0f},
                 0x0000Ff00);
    presentWindow(window);
  }

  // float averageTime = 0.0f;
  // for (size_t i = 0; i < 1000; ++i) {
  //   averageTime += times[i];
  // }
  // printf("averageTime: %f ms\n", averageTime);
  // free(times);

  DestroyModel(&model);
  destroyWindow(window);
  return 0;
}
