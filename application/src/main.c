#include "rendering.h"
#include "window.h"

int main(void) {
  Window* window = createWindow();

  while (!window->ShouldClose) {
    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00FF0070);
    clearDepthBuffer(&window->Framebuffer, 1.0f);
    presentWindow(window);
  }

  destroyWindow(window);
  return 0;
}
