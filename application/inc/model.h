#ifndef SOFTWARE_MODEL_VISUALIZER_MODEL_H
#define SOFTWARE_MODEL_VISUALIZER_MODEL_H
#include "graphicsMath.h"

#include <stddef.h>
#include <stdint.h>

typedef struct Model {
  vec3* Vertices;
  size_t VertexCount;
  uint32_t* Indices;
  size_t IndexCount;
  vec3* Normals;
  uint32_t* Colors;
} Model;

bool LoadModel(const char* FilePath, Model* Destination, vec3* Center,
               float* Extent);
void DestroyModel(const Model* Model);

#endif // SOFTWARE_MODEL_VISUALIZER_MODEL_H
