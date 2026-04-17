#ifndef SOFTWARE_MODEL_VISUALIZER_MODEL_H
#define SOFTWARE_MODEL_VISUALIZER_MODEL_H
#include "graphicsMath.h"

#include <stddef.h>
#include <stdint.h>

typedef struct Model {
  struct Vec3* Vertices;
  size_t VertexCount;
  uint32_t* Indices;
  size_t IndexCount;
  struct Vec3* Normals;
  uint32_t* Colors;
} Model;

bool loadModel(const char* FilePath, Model* Destination, struct Vec3* Center,
               float* Extent);
void destroyModel(const Model* Model);

#endif // SOFTWARE_MODEL_VISUALIZER_MODEL_H
