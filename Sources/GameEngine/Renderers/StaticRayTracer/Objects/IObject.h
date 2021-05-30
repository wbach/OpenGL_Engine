#pragma once
#include <Common/Transform.h>
#include <Types.h>
#include <optional>

namespace GameEngine
{
struct Material;

namespace RayTracer
{
class Ray;
class Image;

class IObject
{
public:
    virtual ~IObject() = default;

    virtual uint32 getId() const                             = 0;
    virtual std::optional<float> intersect(const Ray&) const = 0;
    virtual const vec3& getNormal() const                    = 0;
    virtual vec3 getNormal(const vec3&) const                = 0;
    virtual const Material& getMaterial() const              = 0;

    // virtual const vec3& imageMap(const Ray&, const vec3& point, const Image& img) = 0;
    // virtual const Image& getTexture(int id) const                                 = 0;
    // virtual bool checkTexture() const                                             = 0;
};
}  // namespace RayTracer
}  // namespace GameEngine
