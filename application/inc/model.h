#ifndef MODEL_H
#define MODEL_H
#include "cglm/ivec3.h"

typedef struct Model {
  vec3* Vertices;
  size_t VertexCount;
  uint32_t* Indices;
  size_t IndexCount;
  vec3* Normals;
  uint32_t* Colors;
} Model;

bool LoadModel(const char* FilePath, Model* Destination);
void DestroyModel(const Model* Model);

#endif // MODEL_H
