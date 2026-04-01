#ifndef SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#define SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#include <model.h>

#include "cglm/vec3.h"
#include "window.h"

void clearColorBuffer(const Framebuffer* Framebuffer, uint32_t ClearValue);
void clearDepthBuffer(const Framebuffer* Framebuffer, float ClearValue);
void drawPixel(const Framebuffer* Framebuffer, uint32_t X, uint32_t Y,
               uint32_t Color, float Depth);
void drawTriangle(const Framebuffer* Framebuffer, vec3 V0, vec3 V1, vec3 V2,
                  uint32_t Color);
void drawModel(const Framebuffer* Framebuffer, const Model* Model,
               mat4 Transform);

#endif // SOFTWARE_MODEL_VISUALIZER_RENDERING_H
