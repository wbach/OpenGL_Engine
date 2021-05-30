#include "BaseObject.h"

namespace GameEngine
{
namespace RayTracer
{
namespace
{
Utils::IdPool idPool_;
}
BaseObject::BaseObject(const common::Transform& transform, const Material& material)
    : transform_{transform}
    , material_{material}
    , normal_(0, 1, 0)
    , id_{idPool_.getId()}
{
}

BaseObject::~BaseObject()
{
    idPool_.releaseId(id_);
}

const vec3& BaseObject::getNormal() const
{
    return normal_;
}
const Material& BaseObject::getMaterial() const
{
    return material_;
}
uint32 BaseObject::getId() const
{
    return id_;
}
vec3 BaseObject::getNormal(const vec3&) const
{
    return normal_;
}
}  // namespace RayTracer
}  // namespace GameEngine
