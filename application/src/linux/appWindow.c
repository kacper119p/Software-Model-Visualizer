#include "appwindow.h"

#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>

static constexpr int32_t windowDefaultWidth = 640;
static constexpr int32_t windowDefaultHeight = 480;

static void resizeFramebuffer(struct AppWindow* const Window,
                              const uint32_t Width, const uint32_t Height) {
  if (Window->Framebuffer.Width == Width &&
      Window->Framebuffer.Height == Height) {
    return;
  }

  if (Window->Framebuffer.ColorBuffer) {
    munmap(Window->Framebuffer.ColorBuffer, Window->Framebuffer.Width *
                                                Window->Framebuffer.Height *
                                                sizeof(uint32_t));
  }
  if (Window->Framebuffer.DepthBuffer) {
    munmap(Window->Framebuffer.DepthBuffer, Window->Framebuffer.Width *
                                                Window->Framebuffer.Height *
                                                sizeof(float));
  }

  if (Window->Image) {
    Window->Image->data = nullptr;
    XDestroyImage(Window->Image);
    Window->Image = nullptr;
  }

  if (Width == 0 || Height == 0) {
    Window->Framebuffer.Width = 0;
    Window->Framebuffer.Height = 0;
    Window->Framebuffer.ColorBuffer = nullptr;
    Window->Framebuffer.DepthBuffer = nullptr;
    return;
  }

  Window->Framebuffer.Width = Width;
  Window->Framebuffer.Height = Height;
  Window->Framebuffer.ColorBuffer =
      mmap(nullptr, Width * Height * sizeof(uint32_t), PROT_READ | PROT_WRITE,
           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  Window->Framebuffer.DepthBuffer =
      mmap(nullptr, Width * Height * sizeof(float), PROT_READ | PROT_WRITE,
           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(window->Framebuffer.ColorBuffer != MAP_FAILED);
  assert(window->Framebuffer.DepthBuffer != MAP_FAILED);

  Window->Image = XCreateImage(
      Window->Display,
      DefaultVisual(Window->Display, DefaultScreen(Window->Display)), 24,
      ZPixmap, 0, (char*)Window->Framebuffer.ColorBuffer, Width, Height, 32,
      (int32_t)(Width * sizeof(uint32_t)));
  assert(window->Image);
}

void presentWindow(const struct AppWindow* Window) {
  XPutImage(Window->Display, Window->WindowHandle, Window->Gc, Window->Image, 0,
            0, 0, 0, Window->Framebuffer.Width, Window->Framebuffer.Height);
  XFlush(Window->Display);
}

struct AppWindow* createWindow() {
  struct AppWindow* window = calloc(1, sizeof(struct AppWindow));
  assert(window);

  window->Display = XOpenDisplay(nullptr);
  assert(window->Display);

  const int32_t screen = DefaultScreen(window->Display);

  const double dpi = (double)XDisplayWidth(window->Display, screen) /
                     (double)XDisplayWidthMM(window->Display, screen) * 25.4;
  const double scale = dpi > 96.0 ? dpi / 96.0 : 1.0;
  const uint32_t scaledWidth = (uint32_t)(windowDefaultWidth * scale);
  const uint32_t scaledHeight = (uint32_t)(windowDefaultHeight * scale);

  window->WindowHandle = XCreateSimpleWindow(
      window->Display, RootWindow(window->Display, screen), 0, 0, scaledWidth,
      scaledHeight, 0, BlackPixel(window->Display, screen),
      BlackPixel(window->Display, screen));

  XStoreName(window->Display, window->WindowHandle, "Software Rasterizer");
  XSelectInput(window->Display, window->WindowHandle,
               ExposureMask | StructureNotifyMask);

  Atom wmDelete = XInternAtom(window->Display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(window->Display, window->WindowHandle, &wmDelete, 1);

  window->Gc = XCreateGC(window->Display, window->WindowHandle, 0, nullptr);

  Cursor cursor = XCreateFontCursor(window->Display, XC_left_ptr);
  XDefineCursor(window->Display, window->WindowHandle, cursor);
  XFreeCursor(window->Display, cursor);

  XMapWindow(window->Display, window->WindowHandle);
  XFlush(window->Display);

  resizeFramebuffer(window, windowDefaultWidth, windowDefaultHeight);

  return window;
}

void destroyWindow(struct AppWindow** const Window) {
  struct AppWindow* const window = *Window;
  if (window->Image) {
    window->Image->data = nullptr;
    XDestroyImage(window->Image);
  }
  if (window->Framebuffer.ColorBuffer) {
    munmap(window->Framebuffer.ColorBuffer, window->Framebuffer.Width *
                                                window->Framebuffer.Height *
                                                sizeof(uint32_t));
  }
  if (window->Framebuffer.DepthBuffer) {
    munmap(window->Framebuffer.DepthBuffer, window->Framebuffer.Width *
                                                window->Framebuffer.Height *
                                                sizeof(float));
  }
  if (window->Gc) {
    XFreeGC(window->Display, window->Gc);
  }
  if (window->WindowHandle) {
    XDestroyWindow(window->Display, window->WindowHandle);
  }
  if (window->Display) {
    XCloseDisplay(window->Display);
  }
  free(window);
  *Window = nullptr;
}

void peekWindowMessages(struct AppWindow* const Window) {
  const Atom wmDelete = XInternAtom(Window->Display, "WM_DELETE_WINDOW", False);

  while (XPending(Window->Display)) {
    XEvent event;
    XNextEvent(Window->Display, &event);

    switch (event.type) {
    case ClientMessage: {
      if ((Atom)event.xclient.data.l[0] == wmDelete) {
        Window->ShouldClose = true;
      }
      break;
    }
    case ConfigureNotify: {
      const XConfigureEvent* cfg = &event.xconfigure;
      resizeFramebuffer(Window, (uint32_t)cfg->width, (uint32_t)cfg->height);
      break;
    }
    default: {
      break;
    }
    }
  }
}
