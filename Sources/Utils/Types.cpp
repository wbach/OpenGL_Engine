#include "Types.h"

#include <math.h>

#include <random>

const std::string DEFAULT_STRING{};
const vec2 DEFAULT_VEC2{0.f};
const vec3 VECTOR_ZERO{0.f};
const vec3 VECTOR_UP{0.f, 1.f, 0.f};
const vec3 VECTOR_DOWN{0.f, -1.f, 0.f};
const vec3 VECTOR_LEFT{1.f, 0.f, 0.f};
const vec3 VECTOR_RIGHT{-1.f, 0.f, 0.f};
const vec3 VECTOR_FORWARD{0.f, 0.f, 1.f};
const vec3 VECTOR_BACKWARD{0.f, 0.f, -1.f};
const vec3 VECTOR_ONE{1.f};

namespace
{
std::mt19937& get_engine()
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    return engine;
}
}  // namespace

namespace wb
{
std::string to_string(const vec2i& v)
{
    return std::to_string(v.x) + " " + std::to_string(v.y);
}

std::string to_string(const vec3i& v)
{
    return std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z);
}
}  // namespace wb

vec4 ToVec4(const vec3& v3)
{
    return vec4(v3.x, v3.y, v3.z, 1.0f);
}

vec4 ToVec4(const vec3& v3, float w)
{
    return vec4(v3.x, v3.y, v3.z, w);
}

vec4 ToVec4(const vec2& v2, float z, float w)
{
    return vec4(v2.x, v2.y, z, w);
}

bool compare(float x, float y)
{
    return fabsf(x - y) <= std::numeric_limits<float>::epsilon();
}

bool compare(float x, float y, float epsilon)
{
    return fabsf(x - y) <= epsilon;
}

float getRandomFloat()
{
    return getRandomFloat(0.f, 1.f);
}

float getRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(get_engine());
}

int getRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(get_engine());
}

int getRandomInt()
{
    return getRandomInt(0, 1);
}

std::ostream& operator<<(std::ostream& os, const MeasurementValue& mv)
{
    os << mv.value;
    return os;
}

namespace std
{
std::string to_string(const vec2& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y);
}
std::string to_string(const vec2i& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y);
}
std::string to_string(const vec2ui& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y);
}
std::string to_string(const vec3i& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);
}
std::string to_string(const vec3& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);
}
std::string to_string(const vec4& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w);
}
std::string to_string(const Quaternion& v)
{
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w);
}
std::string to_string(const glm::mat4& m)
{
    std::string s{"\n"};
    for (int y = 0; y < 4; y++)
        s += std::to_string(m[y][0]) + " " + std::to_string(m[y][1]) + " " + std::to_string(m[y][2]) + " " +
             std::to_string(m[y][3]) + "\n";
    return s;
}
template <>
string to_string(const optional<string>& maybeValue)
{
    if (maybeValue)
    {
        return *maybeValue;
    }
    return "-";
}
template <>
string to_string(const std::vector<string>& values)
{
    std::string result;
    for (const auto& value : values)
    {
        result += value + ",";
    }
    if (not result.empty())
    {
        result.pop_back();
    }
    return result;
}

static void parse_next(std::stringstream& ss, float& val)
{
    ss >> val;
    if (ss.peek() == ',' || ss.peek() == ' ')
        ss.ignore();
}

static void parse_next(std::stringstream& ss, int& val)
{
    ss >> val;
    if (ss.peek() == ',' || ss.peek() == ' ')
        ss.ignore();
}

static void parse_next(std::stringstream& ss, unsigned int& val)
{
    ss >> val;
    if (ss.peek() == ',' || ss.peek() == ' ')
        ss.ignore();
}

void from_string(const std::string& s, vec2& v)
{
    std::stringstream ss(s);
    parse_next(ss, v.x);
    parse_next(ss, v.y);
}

void from_string(const std::string& s, vec2i& v)
{
    std::stringstream ss(s);
    parse_next(ss, v.x);
    parse_next(ss, v.y);
}

void from_string(const std::string& s, vec2ui& v)
{
    std::stringstream ss(s);
    parse_next(ss, v.x);
    parse_next(ss, v.y);
}

void from_string(const std::string& s, vec3& v)
{
    std::stringstream ss(s);
    parse_next(ss, v.x);
    parse_next(ss, v.y);
    parse_next(ss, v.z);
}

void from_string(const std::string& s, vec3i& v)
{
    std::stringstream ss(s);
    parse_next(ss, v.x);
    parse_next(ss, v.y);
    parse_next(ss, v.z);
}

void from_string(const std::string& s, vec4& v)
{
    std::stringstream ss(s);
    parse_next(ss, v.x);
    parse_next(ss, v.y);
    parse_next(ss, v.z);
    parse_next(ss, v.w);
}

void from_string(const std::string& s, Quaternion& q)
{
    std::stringstream ss(s);
    parse_next(ss, q.x);
    parse_next(ss, q.y);
    parse_next(ss, q.z);
    parse_next(ss, q.w);
}

void from_string(const std::string& s, mat4& m)
{
    std::stringstream ss(s);
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            ss >> m[y][x];
        }
    }
}
}  // namespace std
