#include "model.h"

#include <time.h>

#include "appWindow.h"
#include "rendering.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  srand(time(nullptr));

  AppWindow* window = createWindow();

  while (!window->ShouldClose) {

    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00000000);
    clearDepthBuffer(&window->Framebuffer, 1.0f);

    drawTriangle(&window->Framebuffer, MAKE_VEC3(-0.5f, -0.5f, 0),
                 MAKE_VEC3(0.5f, -0.5f, 0), MAKE_VEC3(0, 0.5f, 0), 0xFFFF0000);
    presentWindow(window);
  }

  destroyWindow(&window);
  return EXIT_SUCCESS;
}
