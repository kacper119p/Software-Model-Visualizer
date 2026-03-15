#ifndef SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#define SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#include <stdint.h>
#include <windows.h>

typedef struct Framebuffer {
  void* ColorBuffer;
  float* DepthBuffer;
  int32_t Width;
  int32_t Height;
  BITMAPINFO BitmapInfo;
} Framebuffer;

typedef struct Window {
  Framebuffer Framebuffer;
  HDC DeviceContext;
  HWND WindowHandle;
  bool ShouldClose;
} Window;

void presentWindow(const Window* Window);
[[nodiscard]] Window* createWindow();
void destroyWindow(Window* Window);
void peekWindowMessages(Window* Window);

#endif // SOFTWARE_MODEL_VISUALIZER_WINDOW_H
