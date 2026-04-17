#include "model.h"

#include <time.h>

#include "appWindow.h"
#include "rendering.h"
#include "timeQuery.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  srand(time(nullptr));

  AppWindow* window = createWindow();

  TimeQuery timeQuery;
  initializeTimeQuery(&timeQuery);

  while (!window->ShouldClose) {
    const float currentTime = getElapsedTime(&timeQuery);

    peekWindowMessages(window);
    clearColorBuffer(&window->Framebuffer, 0x00000000);
    clearDepthBuffer(&window->Framebuffer, 1.0f);

    const vec3 triangleV0 = MAKE_VEC3(-0.5f, -0.333f, 0.0f);
    const vec3 triangleV1 = MAKE_VEC3(0.5f, -0.333f, 0.0f);
    const vec3 triangleV2 = MAKE_VEC3(0.0f, 1.0f, 0.0f);

    const mat4 rotation = makeMat4Rotation(MAKE_VEC3(0, 0, 1), currentTime);
    const vec4 triangleV02 = mat4MulVec4(
        rotation, MAKE_VEC4(triangleV0.X, triangleV0.Y, triangleV0.Z, 1.0f));
    const vec4 triangleV12 = mat4MulVec4(
        rotation, MAKE_VEC4(triangleV1.X, triangleV1.Y, triangleV1.Z, 1.0f));
    const vec4 triangleV22 = mat4MulVec4(
        rotation, MAKE_VEC4(triangleV2.X, triangleV2.Y, triangleV2.Z, 1.0f));

    drawTriangle(&window->Framebuffer,
                 MAKE_VEC3(triangleV02.X, triangleV02.Y, triangleV02.Z),
                 MAKE_VEC3(triangleV12.X, triangleV12.Y, triangleV12.Z),
                 MAKE_VEC3(triangleV22.X, triangleV22.Y, triangleV22.Z),
                 0xFFFF0000);
    presentWindow(window);
  }

  destroyWindow(&window);
  return EXIT_SUCCESS;
}
