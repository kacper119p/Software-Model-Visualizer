#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "window.h"

int main(void) {
  Window* window = createWindow();

  while (!window->ShouldClose) {
    PeekWindowMessages(window);
    if (window->Framebuffer.ColorBuffer) {
      uint32_t* pixel = (uint32_t*)window->Framebuffer.ColorBuffer;
      for (int32_t i = 0;
           i < window->Framebuffer.Width * window->Framebuffer.Height; ++i) {
        *pixel++ = 0x00FF0070; // Magenta color
      }
    }
    presentWindow(window);
  }

  destroyWindow(window);
  return 0;
}