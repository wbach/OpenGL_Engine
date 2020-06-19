#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "../Types.h"
#include "Glm.h"

namespace Utils
{
// static int s_intmone = -1;
static glm::vec2 s_vec2_zero(0);
static glm::vec3 s_vec3_zero(0);
static glm::mat4 s_mat4_one(1.f);

void PrtintMat4(const glm::mat4& mat);
void PrintVector(const std::string& text, const glm::vec3& v);
void PrintVector(const std::string& text, const glm::vec2& v);
void PrintMatrix(const std::string& text, const glm::mat4& m);

bool Compare(const glm::vec2& v1, const glm::vec2& v2);
bool Compare(const glm::vec3& v1, const glm::vec3& v2);

glm::vec2 toScreenSpace(glm::vec2& position, glm::vec2& window_size);
glm::vec3 Vec4ToVec3(const glm::vec4& v);
glm::vec3 ColorLerpRGB(const glm::vec3& c1, const glm::vec3& c2, const float& blend);
glm::vec3 TransformPoint(const glm::vec3& point, const glm::mat4& matrix);
glm::vec3 RGBtoFloat(const glm::vec3& color);
float RGBtoFloat(float rgb);
float RGBtoFloat(uint8 rgb);
glm::vec3 RGBtoFloat(const float& r, const float& g, const float& b);
glm::vec3 CalculateMinimumVector(const glm::vec3& a, const glm::vec3& b);
glm::vec3 BarryCentricVec3(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos);
glm::vec3 RotateObject(const vec3& center, const vec3& point, float angle);

void CalculateBoudnigBox(const std::vector<float>& positions, glm::vec3& bounding_min, glm::vec3& bounding_max,
                         glm::vec3& bounding_size, glm::vec3& bounding_center);

float GetMaxFromVector(const glm::vec3& vector);

mat4 CreateOffset();
mat4 CreateLightViewMatrix(const vec3& direction, const vec3& center);
mat4 CreateOrthoProjectionMatrix(float width, float height, float length);
mat4 CreateTransformationMatrix(const vec3& translation, const Quaternion& rotation);
mat4 CreateTransformationMatrix(const vec3& translation, const Quaternion& rotation, const vec3& scale);
mat4 CreateTransformationMatrix(const vec3& translation, const DegreesVec3& rotation, const vec3& scale);
mat4 CreateTransformationMatrix(const vec3& translation, const RadiansVec3& rotation, const vec3& scale);
mat4 CreateTransformationMatrix(const vec2& translation, const vec2& scale, DegreesFloat rotationZ = 0.f);
mat4 CreateTransformationMatrix(const vec3& translation, const vec2& scale, DegreesFloat rotationZ = 0.f);
mat4 CreateTransformationMatrix(const vec2& translation, const vec2& scale, RadianFloat rotationZ = 0.f);
mat4 CreateTransformationMatrix(const vec3& translation, const vec2& scale, RadianFloat rotationZ = 0.f);

Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float blend);
float BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos);
}  // namespace Utils
