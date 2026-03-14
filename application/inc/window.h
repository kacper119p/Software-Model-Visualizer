#ifndef SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#define SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

struct Framebuffer {
  void* ColorBuffer;
  float* DepthBuffer;
  int32_t Width;
  int32_t Height;
  BITMAPINFO BitmapInfo;
} typedef Framebuffer;

struct Window {
  Framebuffer Framebuffer;
  HDC DeviceContext;
  HWND WindowHandle;
  bool ShouldClose;
} typedef Window;

void presentWindow(const Window* Window);
Window* createWindow();
void destroyWindow(Window* Window);
void PeekWindowMessages(Window* Window);

#endif // SOFTWARE_MODEL_VISUALIZER_WINDOW_H
