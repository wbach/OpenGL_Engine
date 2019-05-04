#include "../math.hpp"
#include "GLMUtils.h"

glm::vec3 Utils::Vec4ToVec3(const glm::vec4& v)
{
    return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 Utils::ColorLerpRGB(const glm::vec3& c1, const glm::vec3& c2, const float& blend)
{
    glm::vec3 color;
    color.x = c1.x + (c2.x - c1.x) * blend;
    color.y = c1.y + (c2.y - c1.y) * blend;
    color.z = c1.z + (c2.z - c1.z) * blend;
    return color;
}

glm::vec3 Utils::TransformPoint(const glm::vec3& point, const glm::mat4& matrix)
{
    glm::vec4 n = matrix * glm::vec4(point, 1.0f);
    return glm::vec3(n.x, n.y, n.z);
}

glm::vec3 Utils::RGBtoFloat(const glm::vec3& color)
{
    return color / 255.f;
}

float Utils::RGBtoFloat(float rgb)
{
    return rgb / 255.f;
}

float Utils::RGBtoFloat(uint8 rgb)
{
    return RGBtoFloat(static_cast<float>(rgb));
}

glm::vec3 Utils::RGBtoFloat(const float& r, const float& g, const float& b)
{
    return RGBtoFloat(glm::vec3(r, g, b));
}

void Utils::PrintVector(const std::string& text, const glm::vec3& v)
{
    std::cout << text << v.x << " " << v.y << " " << v.z << std::endl;
}

void Utils::PrintVector(const std::string& text, const glm::vec2& v)
{
    std::cout << text << v.x << " " << v.y << std::endl;
}

void Utils::PrintMatrix(const std::string& text, const glm::mat4& m)
{
    for (uint32 y = 0; y < 4; y++)
        std::cout << text << m[y][0] << " " << m[y][1] << " " << m[y][2] << " " << m[y][3] << std::endl;
}

bool Utils::Compare(const glm::vec2& v1, const glm::vec2& v2)
{
    return glm::length(v1 - v2) < std::numeric_limits<float>::epsilon();
}

bool Utils::Compare(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::length(v1 - v2) < std::numeric_limits<float>::epsilon();
}

void Utils::PrtintMat4(const glm::mat4& mat)
{
    printf("\nMat:\n %f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n\n", mat[0][0], mat[0][1], mat[0][2],
           mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0],
           mat[3][1], mat[3][2], mat[3][3]);
}

float Utils::GetMaxFromVector(const glm::vec3& vector)
{
    if (vector.x > vector.y && vector.x > vector.z)
        return vector.x;
    else if (vector.y > vector.z)
        return vector.y;
    else
        return vector.z;
}

glm::mat4 Utils::CreateTransformationMatrix(const glm::vec3& translation, const glm::vec3& rotation,
                                            const glm::vec3& scale)
{
    glm::mat4 matrix = glm::mat4(1.0);
    matrix *= glm::translate(translation);
    matrix *= glm::rotate(rotation.x, 1.0f, 0.0f, 0.0f);
    matrix *= glm::rotate(rotation.y, 0.0f, 1.0f, 0.0f);
    matrix *= glm::rotate(rotation.z, 0.0f, 0.0f, 1.0f);
    matrix *= glm::scale(scale.x, scale.y, scale.z);
    return matrix;
}

glm::mat4 Utils::CreateTransformationMatrix(const glm::vec2& translation, const glm::vec2& scale, const float& rotation)
{
    glm::mat4 matrix = glm::mat4(1.0);
    matrix *= glm::translate(glm::vec3(translation, 0.0f));
    matrix *= glm::rotate(rotation, 0.0f, 0.0f, 1.0f);
    matrix *= glm::scale(glm::vec3(scale, 1.0f));
    return matrix;
}

void Utils::CalculateBoudnigBox(const std::vector<float>& positions, glm::vec3& bounding_min, glm::vec3& bounding_max,
                                glm::vec3& bounding_size, glm::vec3& bounding_center)
{
    if (positions.size() == 0)
        return;

    bounding_min.x = bounding_max.x = positions[0];
    bounding_min.y = bounding_max.y = positions[1];
    bounding_min.z = bounding_max.z = positions[2];
    for (uint32 i = 0; i < positions.size(); i += 3)
    {
        if (positions[i] < bounding_min.x)
            bounding_min.x = positions[i];
        if (positions[i] > bounding_max.x)
            bounding_max.x = positions[i];
        if (positions[i + 1] < bounding_min.y)
            bounding_min.y = positions[i + 1];
        if (positions[i + 1] > bounding_max.y)
            bounding_max.y = positions[i + 1];
        if (positions[i + 2] < bounding_min.z)
            bounding_min.z = positions[i + 2];
        if (positions[i + 2] > bounding_max.z)
            bounding_max.z = positions[i + 2];
    }
    bounding_size   = bounding_max - bounding_min;
    bounding_center = (bounding_max + bounding_min) / 2.f;
}

glm::vec3 Utils::CalculateMinimumVector(const glm::vec3& a, const glm::vec3& b)
{
    glm::vec3 result;
    result.x = a.x < b.x ? a.x : b.x;
    result.y = a.y < b.y ? a.y : b.y;
    result.z = a.z < b.z ? a.z : b.z;
    return result;
}

float Utils::BarryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
{
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1  = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2  = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3  = 1.0f - l1 - l2;
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

glm::vec3 Utils::BarryCentricVec3(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos)
{
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1  = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2  = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3  = 1.0f - l1 - l2;
    return glm::vec3(l1, l2, l3);
}

glm::vec3 Utils::RotateObject(const vec3& center, const vec3& point, float angle)
{
    vec3 result;
    float x  = point.x - center.x;
    float z  = point.z - center.z;
    result.x = x * cos(angle) - z * sin(angle) + center.x;
    result.z = x * sin(angle) + z * cos(angle) + center.z;
    result.y = point.y;

    // auto x   = point.x - center.x;
    // auto z = -point.z - center.z;
    // result.x = cos(angle) * (x) - sin(angle) * (z) + center.x;

    // result.z = sin(angle) * (x) + cos(angle) * (z) + center.z;

    // auto distance = glm::length(center - point);
    ////auto horizontal_distance = distance * cos(Utils::ToRadians(destinationPitch)));
    ////auto vertical_distance = distance * sin(Utils::ToRadians(destinationPitch)));

    // float x_offset = (float)(distance * sin(Utils::ToRadians(angle)));
    // float z_offset = (float)(distance * cos(Utils::ToRadians(angle)));
    // result.x       = center.x - x_offset;
    // result.y       = center.y + distance;
    // result.z       = center.z - z_offset;
    return result;
}

glm::mat4 Utils::CreateOffset()
{
    glm::mat4 offset;
    offset *= glm::translate(glm::vec3(0.5f, 0.5f, 0.5f));
    offset *= glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
    return offset;
}

glm::vec2 Utils::toScreenSpace(glm::vec2& position, glm::vec2& window_size)
{
    return glm::vec2((position.x / window_size.x) * 2 - 1, (1 - (position.y / window_size.y)) * 2 - 1);
}

mat4 Utils::CreateOrthoProjectionMatrix(float width, float height, float length)
{
    mat4 projectonMatrix(1.f);
    projectonMatrix[0][0] = 2.f / width;
    projectonMatrix[1][1] = 2.f / height;
    projectonMatrix[2][2] = -2.f / length;
    projectonMatrix[3][3] = 1.f;
    return projectonMatrix;
}

mat4 Utils::CreateLightViewMatrix(glm::vec3 direction, glm::vec3 center)
{
    direction = glm::normalize(direction);
    center *= -1.0f;

    mat4 lightViewMatrix(1.0);

    float length = glm::length(glm::vec2(direction.x, direction.z));
    float pitch  = (float)Utils::ToDegrees(acos(length));

    lightViewMatrix *= glm::rotate(pitch, glm::vec3(1, 0, 0));

    float yaw = (float)Utils::ToDegrees(((float)atan(direction.x / direction.z)));
    yaw       = direction.z > 0 ? yaw - 180 : yaw;

    lightViewMatrix *= glm::rotate((float)-(yaw), glm::vec3(0, 1, 0));
    lightViewMatrix *= glm::translate(center);

    return lightViewMatrix;
}

Quaternion Utils::Interpolate(const Quaternion& a, const Quaternion& b, float blend)
{
    Quaternion result(0, 0, 0, 1);
    float dot    = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    float blendI = 1.f - blend;
    if (dot < 0)
    {
        result.w = blendI * a.w + blend * -b.w;
        result.x = blendI * a.x + blend * -b.x;
        result.y = blendI * a.y + blend * -b.y;
        result.z = blendI * a.z + blend * -b.z;
    }
    else
    {
        result.w = blendI * a.w + blend * b.w;
        result.x = blendI * a.x + blend * b.x;
        result.y = blendI * a.y + blend * b.y;
        result.z = blendI * a.z + blend * b.z;
    }
    result = glm::normalize(result);
    return result;
}
