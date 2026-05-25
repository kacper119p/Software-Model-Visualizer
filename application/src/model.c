/* MIT License
 *
 * Copyright (c) 2026 Kacper Pastuszka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "model.h"

#include <assert.h>
#include <stdlib.h>

#include "cgltf.h"

static inline enum LoadModelResult
cgltfErrorToLoadModelResult(const cgltf_result CgltfResult) {
  switch (CgltfResult) {
  case cgltf_result_file_not_found:
    return LOAD_MODEL_RESULT_FILE_NOT_FOUND;
  case cgltf_result_io_error:
    return LOAD_MODEL_FAILED_TO_READ_FILE;
  case cgltf_result_unknown_format:
  case cgltf_result_invalid_json:
  case cgltf_result_invalid_gltf:
  case cgltf_result_legacy_gltf:
  case cgltf_result_data_too_short:
    return LOAD_MODEL_RESULT_INVALID_FORMAT;
  case cgltf_result_out_of_memory:
    return LOAD_MODEL_RESULT_OUT_OF_MEMORY;
  default:
    assert(false);
    return LOAD_MODEL_UNKNOWN_ERROR;
  }
}

static inline void freeModelLoadingData(
    struct Vec3* const restrict Vertices, uint32_t* const restrict Indices,
    struct Vec3* const restrict Normals, uint32_t* const restrict Colors,
    struct Vec2* const restrict TextureCoords,
    cgltf_data* const restrict CgltfData) {
  free(Vertices);
  free(Indices);
  free(Normals);
  free(Colors);
  free(TextureCoords);
  cgltf_free(CgltfData);
}

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

enum LoadModelResult loadModel(const char* const FilePath,
                               struct Model* const Destination) {
  assert(Destination != nullptr);
  if (Destination == nullptr) {
    return LOAD_MODEL_RESULT_INVALID_TARGET;
  }

  constexpr cgltf_options options = {0};
  cgltf_data* data = nullptr;
  const cgltf_result fileLoadResult =
      cgltf_parse_file(&options, FilePath, &data);
  if (fileLoadResult != cgltf_result_success) {
    return cgltfErrorToLoadModelResult(fileLoadResult);
  }
  const cgltf_result buffersLoadResult =
      cgltf_load_buffers(&options, data, FilePath);
  if (buffersLoadResult != cgltf_result_success) {
    if (data) {
      cgltf_free(data);
    }
    return cgltfErrorToLoadModelResult(fileLoadResult);
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
    assert(false);
    return LOAD_MODEL_RESULT_NO_GEOMETRY_DATA;
  }

  struct Vec3* const restrict vertices =
      calloc(Destination->VertexCount, sizeof(struct Vec3));
  uint32_t* const restrict indices =
      calloc(Destination->IndexCount, sizeof(uint32_t));
  struct Vec3* const restrict normals =
      calloc(Destination->IndexCount / 3, sizeof(struct Vec3));
  uint32_t* const restrict colors =
      calloc(Destination->IndexCount / 3, sizeof(uint32_t));
  struct Vec2* const restrict textureCoords =
      calloc(Destination->VertexCount, sizeof(struct Vec2));

  if (vertices == nullptr || indices == nullptr || normals == nullptr ||
      colors == nullptr || textureCoords == nullptr) {
    freeModelLoadingData(vertices, indices, normals, colors, textureCoords,
                         data);
    return LOAD_MODEL_RESULT_OUT_OF_MEMORY;
  }

  size_t vertexOffset = 0;
  size_t indexOffset = 0;

  for (size_t i = 0; i < data->nodes_count; ++i) {
    const cgltf_node* node = &data->nodes[i];
    if (node->mesh == nullptr) {
      continue;
    }

    struct Mat4 modelMatrix;
    cgltf_node_transform_world(node, (cgltf_float*)&modelMatrix);

    for (size_t j = 0; j < node->mesh->primitives_count; ++j) {
      const cgltf_primitive* primitive = &node->mesh->primitives[j];

      const cgltf_accessor* positionAccessor = nullptr;
      const cgltf_accessor* texCoordAccessor = nullptr;

      for (size_t k = 0; k < primitive->attributes_count; ++k) {
        const cgltf_attribute* attribute = &primitive->attributes[k];
        if (attribute->type == cgltf_attribute_type_position) {
          positionAccessor = attribute->data;
        } else if (attribute->type == cgltf_attribute_type_texcoord) {
          texCoordAccessor = attribute->data;
        }
      }

      if (positionAccessor == nullptr) {
        continue;
      }

      const size_t primitiveVertexCount = positionAccessor->count;
      if (primitive->indices != nullptr) {
        const size_t primitiveIndexCount = primitive->indices->count;
        const bool isMeshTriangulated = primitiveIndexCount % 3 == 0;
        assert(isMeshTriangulated);
        if (!isMeshTriangulated) {
          freeModelLoadingData(vertices, indices, normals, colors,
                               textureCoords, data);
          return LOAD_MODEL_RESULT_NOT_TRIANGULATED;
        }

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

      if (positionAccessor != nullptr) {
        for (uint32_t v = 0; v < primitiveVertexCount; ++v) {
          struct Vec2 textureCoord;
          cgltf_accessor_read_float(texCoordAccessor, v,
                                    (cgltf_float*)&textureCoord, 2);
          textureCoords[vertexOffset + v] = textureCoord;
        }
      } else {
        for (uint32_t v = 0; v < primitiveVertexCount; ++v) {
          textureCoords[vertexOffset + v] = VEC2_ZERO;
        }
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
  Destination->TextureCoords = textureCoords;

  cgltf_free(data);

  calculateAabb(Destination->Vertices, Destination->VertexCount,
                &Destination->AabbMin, &Destination->AabbMax);
  return LOAD_MODEL_RESULT_SUCCESS;
}

bool generateSphereModel(const float Radius, const uint32_t Segments,
                         const uint32_t Rings, struct Model* Destination) {
  assert(Destination != nullptr);

  if (Radius <= 0.0f || Segments < 3 || Rings < 2) {
    return false;
  }

  Destination->VertexCount = (Rings + 1) * (Segments + 1);
  Destination->IndexCount = (Rings - 1) * Segments * 6;

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
    return false;
  }

  size_t vertexIndex = 0;
  for (uint32_t i = 0; i <= Rings; ++i) {
    const float phi = (float)i * Pi / (float)Rings;
    const float y = Radius * cosf(phi);
    const float sinPhi = sinf(phi);

    for (uint32_t j = 0; j <= Segments; ++j) {
      const float theta = (float)j * 2.0f * Pi / (float)Segments;
      const float x = Radius * sinPhi * cosf(theta);
      const float z = Radius * sinPhi * sinf(theta);

      vertices[vertexIndex].X = x;
      vertices[vertexIndex].Y = y;
      vertices[vertexIndex].Z = z;
      vertexIndex++;
    }
  }

  size_t indexOffset = 0;
  for (uint32_t i = 0; i < Rings; ++i) {
    uint32_t k1 = i * (Segments + 1);
    uint32_t k2 = k1 + Segments + 1;

    for (uint32_t j = 0; j < Segments; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices[indexOffset++] = k1;
        indices[indexOffset++] = k1 + 1;
        indices[indexOffset++] = k2;
      }
      if (i != (Rings - 1)) {
        indices[indexOffset++] = k1 + 1;
        indices[indexOffset++] = k2 + 1;
        indices[indexOffset++] = k2;
      }
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

    const struct Vec3 normal = vec3Cross(edge1, edge2);
    normals[i / 3] = vec3Normalize(normal);
  }

  for (size_t i = 0; i < Destination->IndexCount / 3; ++i) {
    colors[i] = getRandomColor();
  }

  Destination->Vertices = vertices;
  Destination->Indices = indices;
  Destination->Normals = normals;
  Destination->Colors = colors;

  calculateAabb(Destination->Vertices, Destination->VertexCount,
                &Destination->AabbMin, &Destination->AabbMax);

  return true;
}

bool generateConeModel(const float Radius, const float Height,
                       const uint32_t Segments, struct Model* Destination) {
  assert(Destination != nullptr);

  if (Radius <= 0.0f || Height <= 0.0f || Segments < 3) {
    return false;
  }

  Destination->VertexCount = 2 + Segments;
  Destination->IndexCount = Segments * 2 * 3;

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
    return false;
  }

  vertices[0] = MAKE_VEC3(0.0f, Height, 0.0f);
  vertices[1] = MAKE_VEC3(0.0f, 0.0f, 0.0f);

  for (uint32_t i = 0; i < Segments; ++i) {
    const float theta = (float)i * 2.0f * Pi / (float)Segments;
    vertices[2 + i] =
        MAKE_VEC3(Radius * cosf(theta), 0.0f, Radius * sinf(theta));
  }

  size_t indexOffset = 0;
  for (uint32_t i = 0; i < Segments; ++i) {
    const uint32_t curr = 2 + i;
    const uint32_t next = 2 + (i + 1) % Segments;

    indices[indexOffset++] = 0;
    indices[indexOffset++] = next;
    indices[indexOffset++] = curr;
    indices[indexOffset++] = 1;
    indices[indexOffset++] = curr;
    indices[indexOffset++] = next;
  }

  for (size_t i = 0; i < Destination->IndexCount; i += 3) {
    const struct Vec3 v0 = vertices[indices[i]];
    const struct Vec3 v1 = vertices[indices[i + 1]];
    const struct Vec3 v2 = vertices[indices[i + 2]];
    const struct Vec3 edge1 = vec3Sub(v1, v0);
    const struct Vec3 edge2 = vec3Sub(v2, v0);
    normals[i / 3] = vec3Normalize(vec3Cross(edge1, edge2));
  }

  for (size_t i = 0; i < Destination->IndexCount / 3; ++i) {
    colors[i] = getRandomColor();
  }

  Destination->Vertices = vertices;
  Destination->Indices = indices;
  Destination->Normals = normals;
  Destination->Colors = colors;
  Destination->TextureCoords = nullptr;

  calculateAabb(Destination->Vertices, Destination->VertexCount,
                &Destination->AabbMin, &Destination->AabbMax);
  return true;
}

bool generateTorusModel(const float MajorRadius, const float MinorRadius,
                        const uint32_t MajorSegments,
                        const uint32_t MinorSegments,
                        struct Model* Destination) {
  assert(Destination != nullptr);

  if (MajorRadius <= 0.0f || MinorRadius <= 0.0f || MajorSegments < 3 ||
      MinorSegments < 3) {
    return false;
  }

  Destination->VertexCount = MajorSegments * MinorSegments;
  Destination->IndexCount = MajorSegments * MinorSegments * 6;

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
    return false;
  }

  for (uint32_t i = 0; i < MajorSegments; ++i) {
    const float phi = (float)i * 2.0f * Pi / (float)MajorSegments;
    const float cosPhi = cosf(phi);
    const float sinPhi = sinf(phi);

    for (uint32_t j = 0; j < MinorSegments; ++j) {
      const float theta = (float)j * 2.0f * Pi / (float)MinorSegments;
      const float r = MajorRadius + MinorRadius * cosf(theta);
      vertices[i * MinorSegments + j] =
          MAKE_VEC3(r * cosPhi, MinorRadius * sinf(theta), r * sinPhi);
    }
  }

  size_t indexOffset = 0;
  for (uint32_t i = 0; i < MajorSegments; ++i) {
    const uint32_t nextI = (i + 1) % MajorSegments;
    for (uint32_t j = 0; j < MinorSegments; ++j) {
      const uint32_t nextJ = (j + 1) % MinorSegments;
      const uint32_t a = i * MinorSegments + j;
      const uint32_t b = i * MinorSegments + nextJ;
      const uint32_t c = nextI * MinorSegments + j;
      const uint32_t d = nextI * MinorSegments + nextJ;

      indices[indexOffset++] = a;
      indices[indexOffset++] = b;
      indices[indexOffset++] = c;

      indices[indexOffset++] = b;
      indices[indexOffset++] = d;
      indices[indexOffset++] = c;
    }
  }

  for (size_t i = 0; i < Destination->IndexCount; i += 3) {
    const struct Vec3 v0 = vertices[indices[i]];
    const struct Vec3 v1 = vertices[indices[i + 1]];
    const struct Vec3 v2 = vertices[indices[i + 2]];
    const struct Vec3 edge1 = vec3Sub(v1, v0);
    const struct Vec3 edge2 = vec3Sub(v2, v0);
    normals[i / 3] = vec3Normalize(vec3Cross(edge1, edge2));
  }

  for (size_t i = 0; i < Destination->IndexCount / 3; ++i) {
    colors[i] = getRandomColor();
  }

  Destination->Vertices = vertices;
  Destination->Indices = indices;
  Destination->Normals = normals;
  Destination->Colors = colors;
  Destination->TextureCoords = nullptr;

  calculateAabb(Destination->Vertices, Destination->VertexCount,
                &Destination->AabbMin, &Destination->AabbMax);
  return true;
}

bool generateTorusKnotModel(const float Radius, const float TubeRadius,
                            const uint32_t P, const uint32_t Q,
                            const uint32_t CurveSegments,
                            const uint32_t TubeSegments,
                            struct Model* Destination) {
  assert(Destination != nullptr);

  if (Radius <= 0.0f || TubeRadius <= 0.0f || P < 1 || Q < 1 ||
      CurveSegments < 3 || TubeSegments < 3) {
    return false;
  }

  Destination->VertexCount = CurveSegments * TubeSegments;
  Destination->IndexCount = CurveSegments * TubeSegments * 6;

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
    return false;
  }

  struct Vec3* const centers = malloc(CurveSegments * sizeof(struct Vec3));
  struct Vec3* const tangents = malloc(CurveSegments * sizeof(struct Vec3));
  struct Vec3* const normals2 = malloc(CurveSegments * sizeof(struct Vec3));
  struct Vec3* const binormals = malloc(CurveSegments * sizeof(struct Vec3));

  if (centers == nullptr || tangents == nullptr || normals2 == nullptr ||
      binormals == nullptr) {
    free(vertices);
    free(indices);
    free(normals);
    free(colors);
    free(centers);
    free(tangents);
    free(normals2);
    free(binormals);
    return false;
  }

  for (uint32_t i = 0; i < CurveSegments; ++i) {
    const float t = (float)i * 2.0f * Pi / (float)CurveSegments;
    const float tp = (float)(i + 1) * 2.0f * Pi / (float)CurveSegments;

    const float pf = (float)P;
    const float qf = (float)Q;

    const float r = Radius * (cosf(qf * t) + 2.0f);
    centers[i] =
        MAKE_VEC3(r * cosf(pf * t), -Radius * sinf(qf * t), r * sinf(pf * t));

    const float rp = Radius * (cosf(qf * tp) + 2.0f);
    const struct Vec3 next = MAKE_VEC3(
        rp * cosf(pf * tp), -Radius * sinf(qf * tp), rp * sinf(pf * tp));

    tangents[i] = vec3Normalize(vec3Sub(next, centers[i]));
  }

  for (uint32_t i = 0; i < CurveSegments; ++i) {
    const struct Vec3 t = tangents[i];
    struct Vec3 up = VEC3_UP;
    if (fabsf(vec3Dot(t, up)) > 0.99f) {
      up = VEC3_RIGHT;
    }
    normals2[i] = vec3Normalize(vec3Cross(t, up));
    binormals[i] = vec3Cross(t, normals2[i]);
  }

  for (uint32_t i = 0; i < CurveSegments; ++i) {
    for (uint32_t j = 0; j < TubeSegments; ++j) {
      const float theta = (float)j * 2.0f * Pi / (float)TubeSegments;
      const float cosTheta = cosf(theta);
      const float sinTheta = sinf(theta);

      vertices[i * TubeSegments + j] =
          MAKE_VEC3(centers[i].X + TubeRadius * (cosTheta * normals2[i].X +
                                                 sinTheta * binormals[i].X),
                    centers[i].Y + TubeRadius * (cosTheta * normals2[i].Y +
                                                 sinTheta * binormals[i].Y),
                    centers[i].Z + TubeRadius * (cosTheta * normals2[i].Z +
                                                 sinTheta * binormals[i].Z));
    }
  }

  free(centers);
  free(tangents);
  free(normals2);
  free(binormals);

  size_t indexOffset = 0;
  for (uint32_t i = 0; i < CurveSegments; ++i) {
    const uint32_t nextI = (i + 1) % CurveSegments;
    for (uint32_t j = 0; j < TubeSegments; ++j) {
      const uint32_t nextJ = (j + 1) % TubeSegments;
      const uint32_t a = i * TubeSegments + j;
      const uint32_t b = i * TubeSegments + nextJ;
      const uint32_t c = nextI * TubeSegments + j;
      const uint32_t d = nextI * TubeSegments + nextJ;

      indices[indexOffset++] = a;
      indices[indexOffset++] = b;
      indices[indexOffset++] = c;

      indices[indexOffset++] = b;
      indices[indexOffset++] = d;
      indices[indexOffset++] = c;
    }
  }

  for (size_t i = 0; i < Destination->IndexCount; i += 3) {
    const struct Vec3 v0 = vertices[indices[i]];
    const struct Vec3 v1 = vertices[indices[i + 1]];
    const struct Vec3 v2 = vertices[indices[i + 2]];
    const struct Vec3 edge1 = vec3Sub(v1, v0);
    const struct Vec3 edge2 = vec3Sub(v2, v0);
    normals[i / 3] = vec3Normalize(vec3Cross(edge1, edge2));
  }

  for (size_t i = 0; i < Destination->IndexCount / 3; ++i) {
    colors[i] = getRandomColor();
  }

  Destination->Vertices = vertices;
  Destination->Indices = indices;
  Destination->Normals = normals;
  Destination->Colors = colors;
  Destination->TextureCoords = nullptr;

  calculateAabb(Destination->Vertices, Destination->VertexCount,
                &Destination->AabbMin, &Destination->AabbMax);
  return true;
}

void destroyModel(const struct Model* const Model) {
  free(Model->Vertices);
  free(Model->Indices);
  free(Model->Normals);
  free(Model->Colors);
  free(Model->TextureCoords);
}
