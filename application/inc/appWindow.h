#ifndef SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#define SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <X11/Xlib.h>
#endif

typedef struct Framebuffer {
  uint32_t* ColorBuffer;
  float* DepthBuffer;
  uint32_t Width;
  uint32_t Height;
#ifdef _WIN32
  BITMAPINFO BitmapInfo;
#endif
} Framebuffer;

typedef struct appWindow {
  Framebuffer Framebuffer;
#ifdef _WIN32
  HWND WindowHandle;
#elif __linux__
  Display* Display;
  Window WindowHandle;
  GC Gc;
  XImage* Image;
#endif
  bool ShouldClose;
} AppWindow;

void presentWindow(const AppWindow* Window);
[[nodiscard]] AppWindow* createWindow();
void destroyWindow(AppWindow** Window);
void peekWindowMessages(AppWindow* Window);

#endif // SOFTWARE_MODEL_VISUALIZER_WINDOW_H
