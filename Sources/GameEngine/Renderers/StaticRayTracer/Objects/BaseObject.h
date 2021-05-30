#pragma once
#include "IObject.h"
#include "GameEngine/Resources/Models/Material.h"
#include <Utils/IdPool.h>

namespace GameEngine
{
namespace RayTracer
{
class BaseObject : public IObject
{
public:
    BaseObject(const common::Transform&, const Material&);
    ~BaseObject() override;
    const vec3& getNormal() const final override;
    const Material& getMaterial() const override;
    uint32 getId() const override;
    vec3 getNormal(const vec3& intersectPoint) const override;

protected:
    const common::Transform& transform_;
    const Material& material_;
    vec3 normal_;
    uint32 id_;
};
}  // namespace RayTracer
}  // namespace GameEngine
