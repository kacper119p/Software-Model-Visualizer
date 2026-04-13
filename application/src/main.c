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

    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00000000);
    clearDepthBuffer(&window->Framebuffer, 1.0f);

    drawTriangle(&window->Framebuffer, MAKE_VEC3(-0.5f, -0.5f, 0),
                 MAKE_VEC3(0.5f, -0.5f, 0), MAKE_VEC3(0, 0.5f, 0), 0xFFFF0000);
    presentWindow(window);
  }

  destroyModel(&model);
  destroyWindow(&window);
  return EXIT_SUCCESS;
}
