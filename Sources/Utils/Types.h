#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /* pi */
#endif

typedef unsigned char  uchar;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef char  int8;
typedef short int16;
typedef int   int32;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

namespace wb
{
template<class T>
struct Tvec2
{
    T x;
    T y;

	Tvec2() : Tvec2(0, 0)
    {
    }
	Tvec2(T x) : Tvec2(x, x)
    {
    }
	Tvec2(T x, T y) : x(x), y(y)
    {
    }
};

template<class T>
struct Tvec3
{
    T x;
    T y;
    T z;

	Tvec3() : Tvec3(0){}
	Tvec3(T a):x(a), y(a), z(a){}

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
}

typedef wb::vec2i vec2i;
typedef wb::vec2ui vec2ui;
typedef wb::vec3i vec3i;

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::vec4 vec4;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::fquat Quaternion;

typedef std::common_type_t<std::chrono::steady_clock::duration, std::chrono::steady_clock::duration> Delta;

// Specyfic types for different os

#ifdef USE_GNU
    typedef std::chrono::_V2::system_clock::time_point Timepoint;
#else
    typedef std::chrono::time_point<std::chrono::steady_clock> Timepoint;
#endif

