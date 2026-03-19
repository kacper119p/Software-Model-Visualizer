#include "model.h"

#include <assert.h>

#include "cglm/mat4.h"
#include "cgltf.h"

static uint32_t hsvToRgb(const float Hue, const float Saturation,
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

static inline void calculateAabb(const Model* Model, vec3 Min, vec3 Max) {
  vec3 min;
  vec3 max;

  glm_vec3_copy(Model->Vertices[0], min);
  glm_vec3_copy(Model->Vertices[0], max);

  for (size_t i = 1; i < Model->VertexCount; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      if (Model->Vertices[i][j] < min[j]) {
        min[j] = Model->Vertices[i][j];
      } else if (Model->Vertices[i][j] > max[j]) {
        max[j] = Model->Vertices[i][j];
      }
    }
  }

  glm_vec3_copy(min, Min);
  glm_vec3_copy(max, Max);
}

bool LoadModel(const char* FilePath, Model* Destination, vec3 Center,
               float* Extent) {
  assert(Destination != nullptr);
  constexpr cgltf_options options = {0};
  cgltf_data* data = nullptr;
  if (cgltf_parse_file(&options, FilePath, &data) != cgltf_result_success ||
      cgltf_load_buffers(&options, data, FilePath) != cgltf_result_success) {
    if (data) {
      cgltf_free(data);
    }
    glm_vec3_zero(Center);
    *Extent = 0.0f;
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
    glm_vec3_zero(Center);
    *Extent = 0.0f;
    return false;
  }

  Destination->Vertices = calloc(Destination->VertexCount, sizeof(vec3));
  Destination->Indices = calloc(Destination->IndexCount, sizeof(uint32_t));
  Destination->Normals = calloc(Destination->IndexCount / 3, sizeof(vec3));
  Destination->Colors = calloc(Destination->IndexCount / 3, sizeof(uint32_t));

  if (Destination->Vertices == nullptr || Destination->Indices == nullptr ||
      Destination->Normals == nullptr || Destination->Colors == nullptr) {
    free(Destination->Vertices);
    free(Destination->Indices);
    free(Destination->Normals);
    free(Destination->Colors);
    cgltf_free(data);
    glm_vec3_zero(Center);
    *Extent = 0.0f;
    return false;
  }

  size_t vertexOffset = 0;
  size_t indexOffset = 0;

  for (size_t i = 0; i < data->nodes_count; ++i) {
    const cgltf_node* node = &data->nodes[i];
    if (node->mesh == nullptr)
      continue;

    mat4 modelMatrix;
    cgltf_node_transform_world(node, (float*)modelMatrix);

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
          Destination->Indices[indexOffset + idx] = index + vertexOffset;
        }
        indexOffset += primitiveIndexCount;
      } else {
        for (uint32_t idx = 0; idx < primitiveVertexCount; ++idx) {
          Destination->Indices[indexOffset + idx] = idx + vertexOffset;
        }
        indexOffset += primitiveVertexCount;
      }

      for (uint32_t v = 0; v < primitiveVertexCount; ++v) {
        vec3 localPosition;
        vec3 worldPosition;
        cgltf_accessor_read_float(positionAccessor, v, localPosition, 3);
        glm_mat4_mulv3(modelMatrix, localPosition, 1.0f, worldPosition);
        glm_vec3_copy(worldPosition, Destination->Vertices[vertexOffset + v]);
      }

      vertexOffset += primitiveVertexCount;
    }
  }

  for (size_t i = 0; i < Destination->IndexCount; i += 3) {
    const uint32_t index0 = Destination->Indices[i];
    const uint32_t index1 = Destination->Indices[i + 1];
    const uint32_t index2 = Destination->Indices[i + 2];

    vec3 v0;
    vec3 v1;
    vec3 v2;

    glm_vec3_copy(Destination->Vertices[index0], v0);
    glm_vec3_copy(Destination->Vertices[index1], v1);
    glm_vec3_copy(Destination->Vertices[index2], v2);

    vec3 edge1;
    vec3 edge2;
    glm_vec3_sub(v1, v0, edge1);
    glm_vec3_sub(v2, v0, edge2);

    vec3 normal;
    glm_vec3_cross(edge1, edge2, normal);
    glm_vec3_normalize(normal);

    glm_vec3_copy(normal, Destination->Normals[i / 3]);
  }

  for (size_t i = 0; i < Destination->IndexCount / 3; ++i) {
    Destination->Colors[i] = getRandomColor();
  }

  cgltf_free(data);

  vec3 aabbMin;
  vec3 aabbMax;
  calculateAabb(Destination, aabbMin, aabbMax);
  const float size = glm_vec3_distance(aabbMin, aabbMax);
  Center[0] = (aabbMin[0] + aabbMax[0]) * 0.5f;
  Center[1] = (aabbMin[1] + aabbMax[1]) * 0.5f;
  Center[2] = (aabbMin[2] + aabbMax[2]) * 0.5f;
  *Extent = size / 2.0f;

  return true;
}

void DestroyModel(const Model* const Model) {
  free(Model->Vertices);
  free(Model->Indices);
  free(Model->Normals);
  free(Model->Colors);
}
