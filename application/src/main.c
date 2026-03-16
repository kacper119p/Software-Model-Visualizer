#include "rendering.h"
#include "window.h"

int main(void) {
  Window* window = createWindow();

  while (!window->ShouldClose) {
    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00FF0070);
    clearDepthBuffer(&window->Framebuffer, 1.0f);
    drawTriangle(&window->Framebuffer, (vec3){0.5f, 0.0f, 0.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, (vec3){1.0f, 1.0f, 0.0f},
                 0x0000Ff00);
    presentWindow(window);
  }

  destroyWindow(window);
  return 0;
}
