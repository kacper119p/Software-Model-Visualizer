#ifndef SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#define SOFTWARE_MODEL_VISUALIZER_WINDOW_H
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <X11/Xlib.h>
#endif

struct Framebuffer {
  uint32_t* ColorBuffer;
  float* DepthBuffer;
  uint32_t Width;
  uint32_t Height;
#ifdef _WIN32
  BITMAPINFO BitmapInfo;
#endif
};

struct AppWindow {
  struct Framebuffer Framebuffer;
#ifdef _WIN32
  HWND WindowHandle;
#elif __linux__
  Display* Display;
  Window WindowHandle;
  GC Gc;
  XImage* Image;
#endif
  bool ShouldClose;
};

void presentWindow(const struct AppWindow* Window);
[[nodiscard]] struct AppWindow* createWindow();
void destroyWindow(struct AppWindow** Window);
void peekWindowMessages(struct AppWindow* Window);

#endif // SOFTWARE_MODEL_VISUALIZER_WINDOW_H
