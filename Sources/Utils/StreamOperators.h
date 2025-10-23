#pragma once
#include <optional>
#include <ostream>

#include "Types.h"

// ================= GLM =================
namespace glm
{

// Generic operator for all vec<N,T,Q>
template <int N, typename T, qualifier Q>
inline std::ostream& operator<<(std::ostream& os, const vec<N, T, Q>& v)
{
    os << "vec" << N << "(";
    for (int i = 0; i < N; ++i)
    {
        os << v[i];
        if (i < N - 1)
            os << ", ";
    }
    os << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const mat4& m)
{
    os << "mat4(";
    for (int i = 0; i < 4; ++i)
    {
        os << "[";
        for (int j = 0; j < 4; ++j)
        {
            os << m[i][j];
            if (j < 3)
                os << ", ";
        }
        os << "]";
        if (i < 3)
            os << ", ";
    }
    os << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quat& q)
{
    return os << "quat(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
}

}  // namespace glm

// ================= std::optional =================
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::optional<T>& opt)
{
    if (opt.has_value())
        os << *opt;
    else
        os << "nullopt";
    return os;
}

// ================= Własne typy =================
inline std::ostream& operator<<(std::ostream& os, const RadianFloat& r)
{
    return os << r.value << " rad (" << r.Degrees() << " deg)";
}

inline std::ostream& operator<<(std::ostream& os, const DegreesFloat& d)
{
    return os << d.value << " deg (" << d.Radians() << " rad)";
}

inline std::ostream& operator<<(std::ostream& os, const RadiansVec3& rv)
{
    return os << "RadiansVec3(" << rv.value.x << ", " << rv.value.y << ", " << rv.value.z << ") deg(" << rv.Degrees().x << ", "
              << rv.Degrees().y << ", " << rv.Degrees().z << ")";
}

inline std::ostream& operator<<(std::ostream& os, const DegreesVec3& dv)
{
    return os << "DegreesVec3(" << dv.value.x << ", " << dv.value.y << ", " << dv.value.z << ") rad(" << dv.Radians().x << ", "
              << dv.Radians().y << ", " << dv.Radians().z << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Color& c)
{
    os << "Color(float: r=" << c.value.x << ", g=" << c.value.y << ", b=" << c.value.z << ", a=" << c.value.w << "; "
       << "uint8: r=" << static_cast<int>(c.r()) << ", g=" << static_cast<int>(c.g()) << ", b=" << static_cast<int>(c.b())
       << ", a=" << static_cast<int>(c.a()) << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const MeasurementValue& mv)
{
    return os << "MeasurementValue(\"" << mv.value << "\")";
}

// ================= wb::Tvec =================
namespace wb
{

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Tvec2<T>& v)
{
    return os << "Tvec2(" << v.x << ", " << v.y << ")";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Tvec3<T>& v)
{
    return os << "Tvec3(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Tvec4<T>& v)
{
    return os << "Tvec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

}  // namespace wb
