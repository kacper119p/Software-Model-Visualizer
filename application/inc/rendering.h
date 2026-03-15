#ifndef SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#define SOFTWARE_MODEL_VISUALIZER_RENDERING_H
#include "window.h"

void clearColorBuffer(const Framebuffer* Framebuffer, uint32_t ClearValue);
void clearDepthBuffer(const Framebuffer* Framebuffer, float ClearValue);

#endif // SOFTWARE_MODEL_VISUALIZER_RENDERING_H
