#ifndef SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#define SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#include "appWindow.h"
#include "graphicsMath.h"
#include "model.h"

void clearColorBuffer(const struct Framebuffer* Framebuffer,
                      uint32_t ClearValue);
void clearDepthBuffer(const struct Framebuffer* Framebuffer, float ClearValue);
void drawPixel(const struct Framebuffer* Framebuffer, uint32_t X, uint32_t Y,
               uint32_t Color, float Depth);
void drawLine(const struct Framebuffer* Framebuffer, struct Vec3 V0,
              struct Vec3 V1, uint32_t Color);
void drawTriangle(const struct Framebuffer* Framebuffer, struct Vec3 V0,
                  struct Vec3 V1, struct Vec3 V2, uint32_t Color);
void drawModel(const struct Framebuffer* Framebuffer, const struct Model* Model,
               struct Mat4 Transform);
void drawModelMesh(const struct Framebuffer* Framebuffer,
                   const struct Model* Model, struct Mat4 Transform,
                   uint32_t Color);

#endif // SOFTWARE_MODEL_VISUALIZER_RENDERING_H
