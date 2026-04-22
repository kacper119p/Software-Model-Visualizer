#include "model.h"

#include <assert.h>
#include <stdlib.h>

#include "cgltf.h"

static inline uint32_t hsvToRgb(const float Hue, const float Saturation,
                                const float Value) {

  const float hueQuotient = Hue * (1.0f / 60.0f);

  const float c = Value * Saturation;
  const float x = c * (1.0f - fabsf(fmodf(hueQuotient, 2.0f) - 1.0f));

  float red;
  float green;
  float blue;
  const uint8_t sector = (uint8_t)hueQuotient;

  switch (sector) {
  case 0: {
    red = c;
    green = x;
    blue = 0.0f;
    break;
  }
  case 1: {
    red = x;
    green = c;
    blue = 0.0f;
    break;
  }
  case 2: {
    red = 0.0f;
    green = c;
    blue = x;
    break;
  }
  case 3: {
    red = 0.0f;
    green = x;
    blue = c;
    break;
  }
  case 4: {
    red = x;
    green = 0.0f;
    blue = c;
    break;
  }
  case 5:
  default: {
    red = c;
    green = 0.0f;
    blue = x;
    break;
  }
  }

  const float m = Value - c;
  const uint8_t r = (uint8_t)((red + m) * 255.0f);
  const uint8_t g = (uint8_t)((green + m) * 255.0f);
  const uint8_t b = (uint8_t)((blue + m) * 255.0f);

  return (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b;
}

static inline uint32_t getRandomColor() {
  const float hue = (float)rand() / (float)RAND_MAX * 360.0f;
  return hsvToRgb(hue, 1.0f, 1.0f);
}

static inline void calculateAabb(const struct Vec3* const Vertices,
                                 const size_t VertexCount, struct Vec3* Min,
                                 struct Vec3* Max) {
  struct Vec3 min = Vertices[0];
  struct Vec3 max = Vertices[0];

  for (size_t i = 1; i < VertexCount; ++i) {
    const struct Vec3 vertex = Vertices[i];
    if (vertex.X < min.X) {
      min.X = vertex.X;
    } else if (vertex.X > max.X) {
      max.X = vertex.X;
    }
    if (vertex.Y < min.Y) {
      min.Y = vertex.Y;
    } else if (vertex.Y > max.Y) {
      max.Y = vertex.Y;
    }
    if (vertex.Z < min.Z) {
      min.Z = vertex.Z;
    } else if (vertex.Z > max.Z) {
      max.Z = vertex.Z;
    }
  }

  vec3Copy(min, Min);
  vec3Copy(max, Max);
}

bool loadModel(const char* const FilePath, struct Model* const Destination) {
  assert(Destination != nullptr);
  constexpr cgltf_options options = {0};
  cgltf_data* data = nullptr;
  if (cgltf_parse_file(&options, FilePath, &data) != cgltf_result_success ||
      cgltf_load_buffers(&options, data, FilePath) != cgltf_result_success) {
    if (data) {
      cgltf_free(data);
    }
    return false;
  }

  Destination->VertexCount = 0;
  Destination->IndexCount = 0;

  for (size_t i = 0; i < data->nodes_count; ++i) {
    const cgltf_node* node = &data->nodes[i];
    if (node->mesh == nullptr) {
      continue;
    }

    for (size_t j = 0; j < node->mesh->primitives_count; ++j) {
      const cgltf_primitive* primitive = &node->mesh->primitives[j];
      const cgltf_accessor* positionAccessor = nullptr;
      for (size_t k = 0; k < primitive->attributes_count; ++k) {
        if (primitive->attributes[k].type == cgltf_attribute_type_position) {
          positionAccessor = primitive->attributes[k].data;
          break;
        }
      }

      if (positionAccessor != nullptr) {
        Destination->VertexCount += positionAccessor->count;

        if (primitive->indices != nullptr) {
          Destination->IndexCount += primitive->indices->count;
        } else {
          Destination->IndexCount += positionAccessor->count;
        }
      }
    }
  }
  if (Destination->VertexCount == 0) {
    cgltf_free(data);
    return false;
  }

  struct Vec3* const restrict vertices =
      calloc(Destination->VertexCount, sizeof(struct Vec3));
  uint32_t* const restrict indices =
      calloc(Destination->IndexCount, sizeof(uint32_t));
  struct Vec3* const restrict normals =
      calloc(Destination->IndexCount / 3, sizeof(struct Vec3));
  uint32_t* const restrict colors =
      calloc(Destination->IndexCount / 3, sizeof(uint32_t));

  if (vertices == nullptr || indices == nullptr || normals == nullptr ||
      colors == nullptr) {
    free(vertices);
    free(indices);
    free(normals);
    free(colors);
    cgltf_free(data);
    return false;
  }

  size_t vertexOffset = 0;
  size_t indexOffset = 0;

  for (size_t i = 0; i < data->nodes_count; ++i) {
    const cgltf_node* node = &data->nodes[i];
    if (node->mesh == nullptr)
      continue;

    struct Mat4 modelMatrix;
    cgltf_node_transform_world(node, (cgltf_float*)&modelMatrix);

    for (size_t j = 0; j < node->mesh->primitives_count; ++j) {
      const cgltf_primitive* primitive = &node->mesh->primitives[j];

      const cgltf_accessor* positionAccessor = nullptr;

      for (size_t k = 0; k < primitive->attributes_count; ++k) {
        const cgltf_attribute* attribute = &primitive->attributes[k];
        if (attribute->type == cgltf_attribute_type_position)
          positionAccessor = attribute->data;
      }

      if (positionAccessor == nullptr)
        continue;

      const size_t primitiveVertexCount = positionAccessor->count;
      if (primitive->indices != nullptr) {
        const size_t primitiveIndexCount = primitive->indices->count;
        assert(primitiveIndexCount % 3 == 0);
        for (size_t idx = 0; idx < primitiveIndexCount; ++idx) {
          uint32_t index;
          cgltf_accessor_read_uint(primitive->indices, idx, &index, 1);
          indices[indexOffset + idx] = index + vertexOffset;
        }
        indexOffset += primitiveIndexCount;
      } else {
        for (uint32_t idx = 0; idx < primitiveVertexCount; ++idx) {
          indices[indexOffset + idx] = idx + vertexOffset;
        }
        indexOffset += primitiveVertexCount;
      }

      for (uint32_t v = 0; v < primitiveVertexCount; ++v) {
        struct Vec3 localPosition;
        cgltf_accessor_read_float(positionAccessor, v,
                                  (cgltf_float*)&localPosition, 3);
        struct Vec4 worldPosition =
            mat4MulVec4(modelMatrix, MAKE_VEC4(localPosition.X, localPosition.Y,
                                               localPosition.Z, 1.0f));
        vertices[vertexOffset + v] =
            MAKE_VEC3(worldPosition.X, worldPosition.Y, worldPosition.Z);
      }

      vertexOffset += primitiveVertexCount;
    }
  }

  for (size_t i = 0; i < Destination->IndexCount; i += 3) {
    const uint32_t index0 = indices[i];
    const uint32_t index1 = indices[i + 1];
    const uint32_t index2 = indices[i + 2];

    const struct Vec3 v0 = vertices[index0];
    const struct Vec3 v1 = vertices[index1];
    const struct Vec3 v2 = vertices[index2];

    const struct Vec3 edge1 = vec3Sub(v1, v0);
    const struct Vec3 edge2 = vec3Sub(v2, v0);

    struct Vec3 normal = vec3Cross(edge1, edge2);
    normals[i / 3] = vec3Normalize(normal);
  }

  for (size_t i = 0; i < Destination->IndexCount / 3; ++i) {
    colors[i] = getRandomColor();
  }

  Destination->Vertices = vertices;
  Destination->Indices = indices;
  Destination->Normals = normals;
  Destination->Colors = colors;

  cgltf_free(data);

  calculateAabb(Destination->Vertices, Destination->VertexCount,
                &Destination->AabbMin, &Destination->AabbMax);
  return true;
}

void destroyModel(const struct Model* const Model) {
  free(Model->Vertices);
  free(Model->Indices);
  free(Model->Normals);
  free(Model->Colors);
}
