#pragma once
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /* pi */
#endif

// ==================== Typy podstawowe ====================
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

typedef std::string InputFileName;
typedef std::string OutputFileName;
typedef uint32_t IdType;

using DeltaTime = float;

static constexpr IdType INVALID_ID = std::numeric_limits<IdType>::max();

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

// ==================== Czas ====================
#if defined(USE_GNU) || defined(USE_MINGW)
typedef std::chrono::_V2::system_clock::time_point Timepoint;
#else
#define not !
#define and &&
#define or ||
typedef std::chrono::time_point<std::chrono::steady_clock> Timepoint;
#endif

// ==================== Utils ====================
bool compare(float, float);
bool compare(float, float, float);

float getRandomFloat();
float getRandomFloat(float, float);

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

// ==================== Własne wektory ====================
namespace wb
{

template <class T>
struct Tvec2
{
    T x, y;
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

    Tvec2 operator+(const Tvec2& l)
    {
        return Tvec2(x + l.x, y + l.y);
    }
    bool operator==(const Tvec2& v) const
    {
        return x == v.x and y == v.y;
    }
    bool operator!=(const Tvec2& v) const
    {
        return !(*this == v);
    }
    bool operator<(const Tvec2& a) const
    {
        if (x < a.x)
            return true;
        if (x == a.x)
            return y < a.y;
        return false;
    }
};

template <class T>
struct Tvec3
{
    T x, y, z;
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
    Tvec3(T a, T b, T c)
        : x(a)
        , y(b)
        , z(c)
    {
    }

    bool operator==(const Tvec3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }
    bool operator<(const Tvec3& v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }
};

template <class T>
struct Tvec4
{
    T x, y, z, w;
    Tvec4()
        : Tvec4(0)
    {
    }
    Tvec4(T a)
        : x(a)
        , y(a)
        , z(a)
        , w(a)
    {
    }
    Tvec4(T a, T b, T c, T d)
        : x(a)
        , y(b)
        , z(c)
        , w(d)
    {
    }

    bool operator==(const Tvec4& v) const
    {
        return x == v.x and y == v.y and z == v.z and w == v.w;
    }
    bool operator<(const Tvec4& v) const
    {
        return x != v.x or y != v.y or z != v.z or w != v.w;
    }
};

typedef Tvec3<uint8> vec3ui8;
typedef Tvec4<uint8> vec4ui8;

}  // namespace wb

typedef wb::vec3ui8 vec3ui8;
typedef wb::vec4ui8 vec4ui8;

// ==================== Alias na GLM ====================
typedef glm::vec2 vec2;
typedef glm::dvec2 vec2d;
typedef glm::ivec2 vec2i;
typedef glm::uvec2 vec2ui;
typedef glm::vec3 vec3;
typedef glm::dvec3 vec3d;
typedef glm::ivec3 vec3i;
typedef glm::uvec3 vec3ui;
typedef glm::vec4 vec4;
typedef glm::ivec4 vec4i;
typedef glm::uvec4 vec4ui;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::fquat Quaternion;

typedef vec2ui WindowSize;

// ==================== Stałe ====================
extern const std::string DEFAULT_STRING;
extern const vec3 VECTOR_ZERO;
extern const vec2 DEFAULT_VEC2;
extern const vec3 VECTOR_UP;
extern const vec3 VECTOR_DOWN;
extern const vec3 VECTOR_LEFT;
extern const vec3 VECTOR_RIGHT;
extern const vec3 VECTOR_FORWARD;
extern const vec3 VECTOR_BACKWARD;
extern const vec3 VECTOR_ONE;

// ==================== Konwersje ====================
template <class type>
type ToRadians(type a)
{
    return a * static_cast<float>(M_PI) / 180.0f;
}

template <class type>
type ToDegrees(type a)
{
    return a * 180.0f / static_cast<float>(M_PI);
}

// ==================== Radian/Degrees wrappers ====================
struct RadianFloat
{
    RadianFloat()
        : value(0.f)
    {
    }
    RadianFloat(float v)
        : value(v)
    {
    }
    float Degrees() const
    {
        return ToDegrees(value);
    }
    float& operator*()
    {
        return value;
    }
    float* operator->()
    {
        return &value;
    }
    float value;
};

struct RadiansVec3
{
    RadiansVec3()
        : value(0.f)
    {
    }
    RadiansVec3(float x, float y, float z)
        : value(x, y, z)
    {
    }
    RadiansVec3(float v)
        : value(v)
    {
    }
    RadiansVec3(const vec3& v)
        : value(v)
    {
    }
    vec3 Degrees() const
    {
        return ToDegrees(value);
    }
    vec3& operator*()
    {
        return value;
    }
    vec3* operator->()
    {
        return &value;
    }
    vec3 value;
};

struct DegreesFloat
{
    DegreesFloat()
        : value(0.f)
    {
    }
    DegreesFloat(float v)
        : value(v)
    {
    }
    float Radians() const
    {
        return ToRadians(value);
    }
    float& operator*()
    {
        return value;
    }
    float* operator->()
    {
        return &value;
    }
    float value;
};

struct DegreesVec3
{
    DegreesVec3()
        : value(0.f)
    {
    }
    DegreesVec3(float x, float y, float z)
        : value(x, y, z)
    {
    }
    DegreesVec3(float v)
        : value(v)
    {
    }
    DegreesVec3(const vec3& v)
        : value(v)
    {
    }
    vec3 Radians() const
    {
        return ToRadians(value);
    }
    vec3& operator*()
    {
        return value;
    }
    vec3* operator->()
    {
        return &value;
    }
    vec3 value;
};

// ==================== Kolor ====================
struct Color
{
    Color()
        : value(1.f)
    {
    }
    Color(float v)
        : value(v)
    {
    }
    Color(const vec4& floatColor)
        : value(floatColor)
    {
    }
    Color(const vec3& floatColor)
        : value(floatColor, 1.f)
    {
    }
    Color(const vec3ui8& color)
    {
        rgb(color);
        value.w = 1.f;
    }
    Color(const vec4ui8& color)
    {
        rgba(color);
    }
    Color(int v)
        : Color(v, v, v, v)
    {
    }
    Color(int r, int g, int b)
        : Color(r, g, b, 255)
    {
    }
    Color(int r, int g, int b, int a)
        : value(static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f,
                static_cast<float>(a) / 255.f)
    {
    }
    Color(float r, float g, float b)
        : Color(r, g, b, 1.f)
    {
    }
    Color(float r, float g, float b, float a)
        : value(r, g, b, a)
    {
    }

    void r(uint8 r)
    {
        value.x = static_cast<float>(r) / 255.f;
    }
    void g(uint8 g)
    {
        value.y = static_cast<float>(g) / 255.f;
    }
    void b(uint8 b)
    {
        value.z = static_cast<float>(b) / 255.f;
    }
    void a(uint8 a)
    {
        value.w = static_cast<float>(a) / 255.f;
    }
    void r(float r)
    {
        value.x = r;
    }
    void g(float g)
    {
        value.y = g;
    }
    void b(float b)
    {
        value.z = b;
    }
    void a(float a)
    {
        value.w = a;
    }

    void rgb(const vec3ui8& color)
    {
        r(color.x);
        g(color.y);
        b(color.z);
    }
    void rgba(const vec4ui8& color)
    {
        r(color.x);
        g(color.y);
        b(color.z);
        a(color.w);
    }

    uint8 r() const
    {
        return static_cast<uint8>(value.x * 255.f);
    }
    uint8 g() const
    {
        return static_cast<uint8>(value.y * 255.f);
    }
    uint8 b() const
    {
        return static_cast<uint8>(value.z * 255.f);
    }
    uint8 a() const
    {
        return static_cast<uint8>(value.w * 255.f);
    }

    vec3ui rgb() const
    {
        return vec3ui(r(), g(), b());
    }
    vec4ui rgba() const
    {
        return vec4ui(r(), g(), b(), a());
    }

    float& operator[](int idx)
    {
        return value[idx];
    }
    float operator[](int idx) const
    {
        return value[idx];
    }
    vec4* operator->()
    {
        return &value;
    }
    vec4& operator*()
    {
        return value;
    }
    void operator*=(float v)
    {
        value *= v;
    }
    void operator+=(const Color& v)
    {
        value += v.value;
    }

    vec3 xyz() const
    {
        return value;
    }

    vec4 value;
};

// ==================== Inne ====================
vec4 ToVec4(const vec3& v3);
vec4 ToVec4(const vec3& v3, float w);
vec4 ToVec4(const vec2& v2, float z, float w);

struct MeasurementValue
{
    MeasurementValue() = default;
    MeasurementValue(const std::string& v)
        : value(v)
    {
    }
    MeasurementValue(const MeasurementValue&) = delete;
    MeasurementValue(MeasurementValue&&)      = delete;
    void operator=(const std::string& v)
    {
        value = v;
    }
    std::string value;
};


namespace glm
{

constexpr float EPSILON = 1e-6f;

inline bool float_equal(float a, float b, float eps = EPSILON)
{
    return std::fabs(a - b) < eps;
}

// ===== vec2 =====
inline vec2ui operator+(const vec2ui& a, const vec2ui& b)
{
    return vec2ui{a.x + b.x , a.y + b.y};
}

inline bool operator==(const vec2& a, const vec2& b)
{
    return float_equal(a.x, b.x) && float_equal(a.y, b.y);
}
inline bool operator!=(const vec2& a, const vec2& b)
{
    return !(a == b);
}
inline bool operator<(const vec2& a, const vec2& b)
{
    if (!float_equal(a.x, b.x))
        return a.x < b.x;
    if (!float_equal(a.y, b.y))
        return a.y < b.y;
    return false;
}
inline bool operator>(const vec2& a, const vec2& b)
{
    return b < a;
}
inline bool operator<=(const vec2& a, const vec2& b)
{
    return !(b < a);
}
inline bool operator>=(const vec2& a, const vec2& b)
{
    return !(a < b);
}

inline bool operator==(const ivec2& a, const ivec2& b)
{
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const ivec2& a, const ivec2& b)
{
    return !(a == b);
}
inline bool operator<(const ivec2& a, const ivec2& b)
{
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}
inline bool operator>(const ivec2& a, const ivec2& b)
{
    return b < a;
}
inline bool operator<=(const ivec2& a, const ivec2& b)
{
    return !(b < a);
}
inline bool operator>=(const ivec2& a, const ivec2& b)
{
    return !(a < b);
}

inline bool operator==(const uvec2& a, const uvec2& b)
{
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const uvec2& a, const uvec2& b)
{
    return !(a == b);
}
inline bool operator<(const uvec2& a, const uvec2& b)
{
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}
inline bool operator>(const uvec2& a, const uvec2& b)
{
    return b < a;
}
inline bool operator<=(const uvec2& a, const uvec2& b)
{
    return !(b < a);
}
inline bool operator>=(const uvec2& a, const uvec2& b)
{
    return !(a < b);
}

// ===== vec3 =====
inline bool operator==(const vec3& a, const vec3& b)
{
    return float_equal(a.x, b.x) && float_equal(a.y, b.y) && float_equal(a.z, b.z);
}
inline bool operator!=(const vec3& a, const vec3& b)
{
    return !(a == b);
}
inline bool operator<(const vec3& a, const vec3& b)
{
    if (!float_equal(a.x, b.x))
        return a.x < b.x;
    if (!float_equal(a.y, b.y))
        return a.y < b.y;
    if (!float_equal(a.z, b.z))
        return a.z < b.z;
    return false;
}
inline bool operator>(const vec3& a, const vec3& b)
{
    return b < a;
}
inline bool operator<=(const vec3& a, const vec3& b)
{
    return !(b < a);
}
inline bool operator>=(const vec3& a, const vec3& b)
{
    return !(a < b);
}

inline bool operator==(const ivec3& a, const ivec3& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const ivec3& a, const ivec3& b)
{
    return !(a == b);
}
inline bool operator<(const ivec3& a, const ivec3& b)
{
    if (a.x != b.x)
        return a.x < b.x;
    if (a.y != b.y)
        return a.y < b.y;
    return a.z < b.z;
}
inline bool operator>(const ivec3& a, const ivec3& b)
{
    return b < a;
}
inline bool operator<=(const ivec3& a, const ivec3& b)
{
    return !(b < a);
}
inline bool operator>=(const ivec3& a, const ivec3& b)
{
    return !(a < b);
}

inline bool operator==(const uvec3& a, const uvec3& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const uvec3& a, const uvec3& b)
{
    return !(a == b);
}
inline bool operator<(const uvec3& a, const uvec3& b)
{
    if (a.x != b.x)
        return a.x < b.x;
    if (a.y != b.y)
        return a.y < b.y;
    return a.z < b.z;
}
inline bool operator>(const uvec3& a, const uvec3& b)
{
    return b < a;
}
inline bool operator<=(const uvec3& a, const uvec3& b)
{
    return !(b < a);
}
inline bool operator>=(const uvec3& a, const uvec3& b)
{
    return !(a < b);
}

// ===== vec4 =====
inline bool operator==(const vec4& a, const vec4& b)
{
    return float_equal(a.x, b.x) && float_equal(a.y, b.y) &&
           float_equal(a.z, b.z) && float_equal(a.w, b.w);
}
inline bool operator!=(const vec4& a, const vec4& b)
{
    return !(a == b);
}
inline bool operator<(const vec4& a, const vec4& b)
{
    if (!float_equal(a.x, b.x))
        return a.x < b.x;
    if (!float_equal(a.y, b.y))
        return a.y < b.y;
    if (!float_equal(a.z, b.z))
        return a.z < b.z;
    if (!float_equal(a.w, b.w))
        return a.w < b.w;
    return false;
}
inline bool operator>(const vec4& a, const vec4& b)
{
    return b < a;
}
inline bool operator<=(const vec4& a, const vec4& b)
{
    return !(b < a);
}
inline bool operator>=(const vec4& a, const vec4& b)
{
    return !(a < b);
}

inline bool operator==(const ivec4& a, const ivec4& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
inline bool operator!=(const ivec4& a, const ivec4& b)
{
    return !(a == b);
}
inline bool operator<(const ivec4& a, const ivec4& b)
{
    if (a.x != b.x)
        return a.x < b.x;
    if (a.y != b.y)
        return a.y < b.y;
    if (a.z != b.z)
        return a.z < b.z;
    return a.w < b.w;
}
inline bool operator>(const ivec4& a, const ivec4& b)
{
    return b < a;
}
inline bool operator<=(const ivec4& a, const ivec4& b)
{
    return !(b < a);
}
inline bool operator>=(const ivec4& a, const ivec4& b)
{
    return !(a < b);
}

inline bool operator==(const uvec4& a, const uvec4& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
inline bool operator!=(const uvec4& a, const uvec4& b)
{
    return !(a == b);
}
inline bool operator<(const uvec4& a, const uvec4& b)
{
    if (a.x != b.x)
        return a.x < b.x;
    if (a.y != b.y)
        return a.y < b.y;
    if (a.z != b.z)
        return a.z < b.z;
    return a.w < b.w;
}
inline bool operator>(const uvec4& a, const uvec4& b)
{
    return b < a;
}
inline bool operator<=(const uvec4& a, const uvec4& b)
{
    return !(b < a);
}
inline bool operator>=(const uvec4& a, const uvec4& b)
{
    return !(a < b);
}

}  // namespace glm



// ====== Color ======
inline bool operator==(const Color& a, const Color& b)
{
    return a.value == b.value;
}
inline bool operator!=(const Color& a, const Color& b)
{
    return !(a == b);
}
inline bool operator<(const Color& a, const Color& b)
{
    return a.value < b.value;
}

typedef std::common_type_t<std::chrono::steady_clock::duration, std::chrono::steady_clock::duration> Delta;

enum class VertexBufferObjects
{
    INDICES = 0,
    POSITION,
    TEXT_COORD,
    NORMAL,
    TANGENT,
    WEIGHTS,
    JOINTS,
    TRANSFORM_MATRIX,
    TEXTURE_OFFSET,
    BLEND_FACTOR
};

typedef std::unordered_map<VertexBufferObjects, uint32> VboMap;

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

std::string to_string(const vec2&);
std::string to_string(const vec2i&);
std::string to_string(const vec2ui&);
std::string to_string(const vec3i&);
std::string to_string(const vec3&);
std::string to_string(const vec4&);
std::string to_string(const Quaternion&);
std::string to_string(const glm::mat4&);

template <typename T>
std::string to_string(const std::optional<T>& maybeValue)
{
    if (maybeValue)
    {
        return std::to_string(*maybeValue);
    }
    return "-";
}
template <typename T>
std::string to_string(const std::vector<T>& values)
{
    std::string result;
    for (const auto& value : values)
    {
        result += std::to_string(value) + "\n";
    }
    if (not result.empty())
    {
        result.pop_back();
    }
    return result;
}
template <>
std::string to_string(const std::optional<std::string>&);
template <>
std::string to_string(const std::vector<std::string>&);
}  // namespace std
