#ifndef SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#define SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#include <stdint.h>
#include <windows.h>

typedef struct {
  uint32_t* ColorBuffer;
  float* DepthBuffer;
  uint32_t Width;
  uint32_t Height;
  BITMAPINFO BitmapInfo;
} Framebuffer;

typedef struct {
  Framebuffer Framebuffer;
  HWND WindowHandle;
  bool ShouldClose;
} Window;

void presentWindow(const Window* Window);
[[nodiscard]] Window* createWindow();
void destroyWindow(Window* Window);
void peekWindowMessages(Window* Window);

#endif // SOFTWARE_MODEL_VISUALIZER_WINDOW_H
