#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#include "window.h"

int main(void) {
  Window* window = createWindow();
  while (!window->ShouldClose) {
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) {
        window->ShouldClose = true;
      }
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }

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