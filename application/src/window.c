#include "window.h"

#include <assert.h>
#include <stdint.h>
#include <windows.h>

static void resizeFramebuffer(Framebuffer* Framebuffer, int32_t const Width,
                              int32_t const Height) {

  if (Framebuffer->Width == Width && Framebuffer->Height == Height) {
    return;
  }

  if (Framebuffer->ColorBuffer) {
    VirtualFree(Framebuffer->ColorBuffer, 0, MEM_RELEASE);
  }
  if (Framebuffer->DepthBuffer) {
    VirtualFree(Framebuffer->DepthBuffer, 0, MEM_RELEASE);
  }

  Framebuffer->Width = Width;
  Framebuffer->Height = Height;

  Framebuffer->BitmapInfo.bmiHeader.biSize =
      sizeof(Framebuffer->BitmapInfo.bmiHeader);
  Framebuffer->BitmapInfo.bmiHeader.biWidth = Framebuffer->Width;
  Framebuffer->BitmapInfo.bmiHeader.biHeight = -Framebuffer->Height;
  Framebuffer->BitmapInfo.bmiHeader.biPlanes = 1;
  Framebuffer->BitmapInfo.bmiHeader.biBitCount = 32;
  Framebuffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;

  const size_t bitmapMemorySize =
      Framebuffer->Width * Framebuffer->Height * sizeof(uint32_t);
  Framebuffer->ColorBuffer = VirtualAlloc(
      NULL, bitmapMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  assert(Framebuffer->ColorBuffer);

  const size_t depthBufferMemorySize =
      Framebuffer->Width * Framebuffer->Height * sizeof(float);
  Framebuffer->DepthBuffer = VirtualAlloc(
      NULL, depthBufferMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  assert(Framebuffer->DepthBuffer);
}

static LRESULT CALLBACK windowProcedure(HWND WindowHandle, const UINT Message,
                                        const WPARAM WParam,
                                        const LPARAM LParam) {

  switch (Message) {
  case WM_CREATE: {
    const CREATESTRUCT* createStruct = (CREATESTRUCT*)LParam;
    Window* windowPtr = createStruct->lpCreateParams;

    SetWindowLongPtr(WindowHandle, GWLP_USERDATA, (LONG_PTR)windowPtr);
    return 0;
  }
  case WM_SIZE: {
    RECT clientRect;
    GetClientRect(WindowHandle, &clientRect);

    const int32_t width = clientRect.right - clientRect.left;
    const int32_t height = clientRect.bottom - clientRect.top;
    Window* windowPtr = (Window*)GetWindowLongPtr(WindowHandle, GWLP_USERDATA);
    resizeFramebuffer(&windowPtr->Framebuffer, width, height);

    return 0;
  }
  case WM_CLOSE:
  case WM_DESTROY: {
    Window* windowPtr = (Window*)GetWindowLongPtr(WindowHandle, GWLP_USERDATA);
    windowPtr->ShouldClose = true;
    PostQuitMessage(0);
    return 0;
  }
  default: {
    return DefWindowProc(WindowHandle, Message, WParam, LParam);
  }
  }
}

void presentWindow(const Window* const Window) {
  StretchDIBits(Window->DeviceContext, 0, 0, Window->Framebuffer.Width,
                Window->Framebuffer.Width, 0, 0, Window->Framebuffer.Width,
                Window->Framebuffer.Height, Window->Framebuffer.ColorBuffer,
                &Window->Framebuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

Window* createWindow() {
  HINSTANCE instance = GetModuleHandle(NULL);
  WNDCLASSEX windowClass = {0};

  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = windowProcedure;
  windowClass.hInstance = instance;
  windowClass.lpszClassName = "WindowClass";
  windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

  const ATOM registerClassResult = RegisterClassEx(&windowClass);
  assert(registerClassResult);

  Window* window = calloc(1, sizeof(Window));
  assert(window);

  constexpr int32_t width = 640;
  constexpr int32_t height = 480;

  HWND windowHandle = CreateWindowEx(
      WS_EX_LEFT, windowClass.lpszClassName, "Software Rasterizer",
      WS_TILEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
      NULL, NULL, instance, window);

  assert(windowHandle);

  window->ShouldClose = false;
  window->DeviceContext = GetDC(windowHandle);
  window->WindowHandle = windowHandle;
  resizeFramebuffer(&window->Framebuffer, width, height);

  return window;
}

void destroyWindow(Window* Window) {
  if (Window->Framebuffer.ColorBuffer) {
    VirtualFree(Window->Framebuffer.ColorBuffer, 0, MEM_RELEASE);
  }
  if (Window->Framebuffer.DepthBuffer) {
    VirtualFree(Window->Framebuffer.DepthBuffer, 0, MEM_RELEASE);
  }
  if (Window->WindowHandle && Window->DeviceContext) {
    ReleaseDC(Window->WindowHandle, Window->DeviceContext);
    DestroyWindow(Window->WindowHandle);
  }
  free(Window);
}
void peekWindowMessages(Window* Window) {
  MSG message;
  while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
    if (message.message == WM_QUIT) {
      Window->ShouldClose = true;
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}
