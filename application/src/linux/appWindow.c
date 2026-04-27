/* MIT License
 *
 * Copyright (c) 2026 Kacper Pastuszka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "appwindow.h"

#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
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
  assert(Window->Framebuffer.ColorBuffer != MAP_FAILED);
  assert(Window->Framebuffer.DepthBuffer != MAP_FAILED);

  Window->Image = XCreateImage(
      Window->Display,
      DefaultVisual(Window->Display, DefaultScreen(Window->Display)), 24,
      ZPixmap, 0, (char*)Window->Framebuffer.ColorBuffer, Width, Height, 32,
      (int32_t)(Width * sizeof(uint32_t)));
  assert(Window->Image);
}

void presentWindow(const struct AppWindow* const Window) {
  XPutImage(Window->Display, Window->WindowHandle, Window->Gc, Window->Image, 0,
            0, 0, 0, Window->Framebuffer.Width, Window->Framebuffer.Height);
  XFlush(Window->Display);
}

void createWindow(struct AppWindow* const Window) {
  assert(Window != nullptr);
  if (Window == nullptr) {
    return;
  }
  memset(Window, 0, sizeof(struct AppWindow));

  Window->Display = XOpenDisplay(nullptr);
  assert(Window->Display);

  const int32_t screen = DefaultScreen(Window->Display);

  const double dpi = (double)XDisplayWidth(Window->Display, screen) /
                     (double)XDisplayWidthMM(Window->Display, screen) * 25.4;
  const double scale = dpi > 96.0 ? dpi / 96.0 : 1.0;
  const uint32_t scaledWidth = (uint32_t)(windowDefaultWidth * scale);
  const uint32_t scaledHeight = (uint32_t)(windowDefaultHeight * scale);

  Window->WindowHandle = XCreateSimpleWindow(
      Window->Display, RootWindow(Window->Display, screen), 0, 0, scaledWidth,
      scaledHeight, 0, BlackPixel(Window->Display, screen),
      BlackPixel(Window->Display, screen));

  XStoreName(Window->Display, Window->WindowHandle, "Software Rasterizer");
  XSelectInput(Window->Display, Window->WindowHandle,
               ExposureMask | StructureNotifyMask);

  Atom wmDelete = XInternAtom(Window->Display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(Window->Display, Window->WindowHandle, &wmDelete, 1);

  Window->Gc = XCreateGC(Window->Display, Window->WindowHandle, 0, nullptr);

  Cursor cursor = XCreateFontCursor(Window->Display, XC_left_ptr);
  XDefineCursor(Window->Display, Window->WindowHandle, cursor);
  XFreeCursor(Window->Display, cursor);

  XMapWindow(Window->Display, Window->WindowHandle);
  XFlush(Window->Display);

  resizeFramebuffer(Window, windowDefaultWidth, windowDefaultHeight);
}

void destroyWindow(struct AppWindow* const Window) {
  assert(Window != nullptr);
  if (Window == nullptr) {
    return;
  }

  if (Window->Image) {
    Window->Image->data = nullptr;
    XDestroyImage(Window->Image);
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
  if (Window->Gc) {
    XFreeGC(Window->Display, Window->Gc);
  }
  if (Window->WindowHandle) {
    XDestroyWindow(Window->Display, Window->WindowHandle);
  }
  if (Window->Display) {
    XCloseDisplay(Window->Display);
  }
  memset(Window, 0, sizeof(struct AppWindow));
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
