#pragma once
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /* pi */
#endif

typedef unsigned char uchar;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 IndicesDataType;
typedef std::vector<IndicesDataType> IndicesVector;
typedef std::vector<float> FloatAttributeVec;
typedef std::vector<int32> JointsVector;
typedef std::vector<uint8> Uint8Vec;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

template <class T>
struct alignas(16) AlignWrapper
{
    T value;

    AlignWrapper operator=(const T& v)
    {
        this->value = v;
        return *this;
    }
};

namespace wb
{
template <class T>
struct Tvec2
{
    T x;
    T y;

    Tvec2()
        : Tvec2(0, 0)
    {
    }
    Tvec2(T x)
        : Tvec2(x, x)
    {
    }
    Tvec2(T x, T y)
        : x(x)
        , y(y)
    {
    }
};

template <class T>
struct Tvec3
{
    T x;
    T y;
    T z;

    Tvec3()
        : Tvec3(0)
    {
    }
    Tvec3(T a)
        : x(a)
        , y(a)
        , z(a)
    {
    }

    bool operator==(const Tvec3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }
    // To my model map find
    bool operator<(const Tvec3& v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }
};

typedef Tvec2<int32> vec2i;
typedef Tvec2<uint32> vec2ui;
typedef Tvec3<int32> vec3i;
typedef Tvec3<uint32> vec3ui;

std::string to_string(const vec2i& v);
std::string to_string(const vec3i& v);
}  // namespace wb

typedef wb::vec2i vec2i;
typedef wb::vec2ui vec2ui;
typedef wb::vec3i vec3i;

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::vec4 vec4;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::fquat Quaternion;

vec4 ToVec4(const vec3& v3);
vec4 ToVec4(const vec3& v3, float w);
vec4 ToVec4(const vec2& v2, float z, float w);

namespace std
{
std::string to_string(const vec2&);
std::string to_string(const vec2i&);
std::string to_string(const vec2ui&);
std::string to_string(const vec3i&);
std::string to_string(const vec3&);
std::string to_string(const glm::mat4& m);
}  // namespace std

typedef std::common_type_t<std::chrono::steady_clock::duration, std::chrono::steady_clock::duration> Delta;

enum class VertexBufferObjects
{
    INDICES          = 0,
    POSITION         = 1,
    TEXT_COORD       = 2,
    NORMAL           = 3,
    TANGENT          = 4,
    WEIGHTS          = 5,
    JOINTS           = 6,
    TRANSFORM_MATRIX = 7,
    TEXTURE_OFFSET   = 8,
    BLEND_FACTOR     = 9
};

typedef std::unordered_map<VertexBufferObjects, uint32> VboMap;

// Specyfic types for different os

// clang-format off
#ifdef USE_GNU
typedef std::chrono::_V2::system_clock::time_point Timepoint;
#else
#define not !
#define and &&
#define or ||
typedef std::chrono::time_point<std::chrono::steady_clock> Timepoint;
#endif
// clang-format on

namespace std
{
template <>
struct hash<vec3i>
{
    size_t operator()(const vec3i& k) const
    {
        size_t h1 = std::hash<double>()(k.x);
        size_t h2 = std::hash<double>()(k.y);
        size_t h3 = std::hash<double>()(k.z);
        return (h1 ^ (h2 << 1)) ^ h3;
    }
};
}  // namespace std
