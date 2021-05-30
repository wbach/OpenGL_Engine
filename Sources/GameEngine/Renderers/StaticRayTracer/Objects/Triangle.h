#pragma once
#include <Common/Transform.h>
#include "BaseObject.h"
#include "GameEngine/Resources/Models/Material.h"

namespace GameEngine
{
namespace RayTracer
{
class Triangle : public BaseObject
{
public:
    Triangle(const common::Transform&, const Material&, const vec3& v1, const vec3& v2, const vec3& v3);
    Triangle(const common::Transform&, const Material&, const vec3& v1, const vec3& v2, const vec3& v3, const vec3& normal);
    virtual std::optional<float> intersect(const Ray& ray) const final;
    virtual vec3 getNormal(const vec3& intersectPoint) const;
    void setVertexNormals(const vec3& vn1, const vec3& vn2, const vec3& vn3);
    void setUVs(const vec2& vt1, const vec2& vt2, const vec2& vt3);
    void enableSmoothNormals(bool);

private:
    struct VertexData
    {
        vec3 v;
        vec2 vt;
        vec3 vn;

        VertexData()
            : VertexData(vec3(0))
        {
        }
        VertexData(const vec3& v)
            : VertexData(v, vec2(0, 0), vec3(0, 1, 0))
        {
        }
        VertexData(const vec3& v, const vec2& uv, const vec3& vn)
            : v(v)
            , vt(uv)
            , vn(vn)
        {
        }
    };

private:
    vec3 calculateSmoothNormal(const vec2& uv) const;
    vec3 calculateStaticNormal(const vec3& v1, const vec3& v2, const vec3& v3);
    vec2 calculateUV(const vec3& intersectPoint) const;
    bool isInPlanePointInTriangle(const vec2& uv) const;
    void calculateIntersectionConstVariables();

private:
    std::array<VertexData, 3> v_;
    bool useSmoothNormal_;
    float d_;
    float dot00_;
    vec3 v0_;
};
}  // namespace RayTracer
}  // namespace GameEngine
