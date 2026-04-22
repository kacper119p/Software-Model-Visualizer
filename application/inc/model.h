#ifndef SOFTWARE_MODEL_VISUALIZER_MODEL_H
#define SOFTWARE_MODEL_VISUALIZER_MODEL_H
#include "graphicsMath.h"

#include <stddef.h>
#include <stdint.h>

struct Model {
  struct Vec3* Vertices;
  size_t VertexCount;
  uint32_t* Indices;
  size_t IndexCount;
  struct Vec3* Normals;
  uint32_t* Colors;
  struct Vec3 AabbMin;
  struct Vec3 AabbMax;
};

bool loadModel(const char* FilePath, struct Model* Destination);
void destroyModel(const struct Model* Model);

#endif // SOFTWARE_MODEL_VISUALIZER_MODEL_H
