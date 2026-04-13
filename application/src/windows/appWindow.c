#include "appWindow.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

static void resizeFramebuffer(Framebuffer* const Framebuffer,
                              uint32_t const Width, uint32_t const Height) {

  if (Framebuffer->Width == Width && Framebuffer->Height == Height) {
    return;
  }

  if (Framebuffer->ColorBuffer) {
    VirtualFree(Framebuffer->ColorBuffer, 0, MEM_RELEASE);
  }
  if (Framebuffer->DepthBuffer) {
    VirtualFree(Framebuffer->DepthBuffer, 0, MEM_RELEASE);
  }

  if (Width == 0 || Height == 0) {
    Framebuffer->Width = 0;
    Framebuffer->Height = 0;
    Framebuffer->ColorBuffer = nullptr;
    Framebuffer->DepthBuffer = nullptr;
    return;
  }

  Framebuffer->Width = Width;
  Framebuffer->Height = Height;

  Framebuffer->BitmapInfo.bmiHeader.biSize =
      sizeof(Framebuffer->BitmapInfo.bmiHeader);
  Framebuffer->BitmapInfo.bmiHeader.biWidth = Framebuffer->Width;
  Framebuffer->BitmapInfo.bmiHeader.biHeight = -(int32_t)Framebuffer->Height;
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
    AppWindow* window = createStruct->lpCreateParams;
    window->ShouldClose = false;
    window->WindowHandle = WindowHandle;

    SetWindowLongPtr(WindowHandle, GWLP_USERDATA, (LONG_PTR)window);
    return 0;
  }
  case WM_SIZE: {
    RECT clientRect;
    GetClientRect(WindowHandle, &clientRect);

    const uint32_t width = clientRect.right - clientRect.left;
    const uint32_t height = clientRect.bottom - clientRect.top;
    AppWindow* windowPtr =
        (AppWindow*)GetWindowLongPtr(WindowHandle, GWLP_USERDATA);
    resizeFramebuffer(&windowPtr->Framebuffer, width, height);

    return 0;
  }
  case WM_CLOSE:
  case WM_DESTROY: {
    AppWindow* windowPtr =
        (AppWindow*)GetWindowLongPtr(WindowHandle, GWLP_USERDATA);
    windowPtr->ShouldClose = true;
    PostQuitMessage(0);
    return 0;
  }
  default: {
    return DefWindowProc(WindowHandle, Message, WParam, LParam);
  }
  }
}

void presentWindow(const AppWindow* const Window) {
  HDC deviceContext = GetDC(Window->WindowHandle);
  StretchDIBits(deviceContext, 0, 0, Window->Framebuffer.Width,
                Window->Framebuffer.Height, 0, 0, Window->Framebuffer.Width,
                Window->Framebuffer.Height, Window->Framebuffer.ColorBuffer,
                &Window->Framebuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
  ReleaseDC(Window->WindowHandle, deviceContext);
}

AppWindow* createWindow() {
  HINSTANCE instance = GetModuleHandle(NULL);
  WNDCLASSEX windowClass = {0};

  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = windowProcedure;
  windowClass.hInstance = instance;
  windowClass.lpszClassName = "WindowClass";
  windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

#if NDEBUG
  RegisterClassEx(&windowClass);
#else
  const ATOM registerClassResult = RegisterClassEx(&windowClass);
  assert(registerClassResult);
#endif

  AppWindow* window = calloc(1, sizeof(AppWindow));
  assert(window);

  constexpr int32_t width = 640;
  constexpr int32_t height = 480;
  constexpr DWORD windowExStyle = WS_EX_LEFT;
  constexpr DWORD windowStyle = WS_TILEDWINDOW | WS_VISIBLE;

  RECT windowRect = {0, 0, width, height};
  AdjustWindowRectEx(&windowRect, windowStyle, FALSE, windowExStyle);

  const int32_t windowWidth = windowRect.right - windowRect.left;
  const int32_t windowHeight = windowRect.bottom - windowRect.top;
#if !NDEBUG
  HWND windowHandle =
#endif
      CreateWindowEx(windowExStyle, windowClass.lpszClassName,
                     "Software Rasterizer", windowStyle, CW_USEDEFAULT,
                     CW_USEDEFAULT, windowWidth, windowHeight, NULL, NULL,
                     instance, window);
  assert(windowHandle);

  return window;
}

void destroyWindow(AppWindow** const Window) {
  AppWindow* window = *Window;
  if (window->Framebuffer.ColorBuffer) {
    VirtualFree(window->Framebuffer.ColorBuffer, 0, MEM_RELEASE);
  }
  if (window->Framebuffer.DepthBuffer) {
    VirtualFree(window->Framebuffer.DepthBuffer, 0, MEM_RELEASE);
  }
  if (window->WindowHandle) {
    DestroyWindow(window->WindowHandle);
  }
  free(window);
  *Window = nullptr;
}

void peekWindowMessages(AppWindow* const Window) {
  MSG message;
  while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
    if (message.message == WM_QUIT) {
      Window->ShouldClose = true;
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}
