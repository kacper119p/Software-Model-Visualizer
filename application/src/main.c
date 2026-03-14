#include <stdint.h>
#include <stdio.h>

#include "console.h"
#include "framebuffer.h"
#include "rasterizer.h"

int main(void) {
  while (true) {
    int32_t width, height;
    getConsolePixelSize(&width, &height);
    Framebuffer fb;
    framebufferInit(&fb, width, height);
    printf("width = %d, height = %d\n", width, height);
  }
  return 0;
}
