#include "PerspectiveProjection.h"

#include <Logger/Log.h>
#include <memory>
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
namespace
{
const float DEFAULT_FOV{60.f};
}  // namespace
PerspectiveProjection::PerspectiveProjection()
    : PerspectiveProjection(EngineConf.renderer.resolution)
{
}
PerspectiveProjection::PerspectiveProjection(const vec2ui &renderingSize)
    : Projection(renderingSize)
    , fov_(DEFAULT_FOV)
{
    Init();
}
PerspectiveProjection::PerspectiveProjection(const vec2ui &renderingSize, float near, float far, float fov)
    : Projection(renderingSize, near, far)
    , fov_(fov)
{
    Init();
}
PerspectiveProjection::PerspectiveProjection(const PerspectiveProjection &p)
    : Projection(p)
    , fov_(p.fov_)
{
    Init();
}
PerspectiveProjection::~PerspectiveProjection()
{
    UnsubscribeForEvents();
}
PerspectiveProjection &PerspectiveProjection::operator=(const PerspectiveProjection &p)
{
    if (this == &p)
        return *this;

    fov_ = p.fov_;
    Projection::operator=(p);

    Init();

    return *this;
}
void PerspectiveProjection::UpdateMatrix()
{
    matrix_ = glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_, farPlane_);
}
float PerspectiveProjection::GetFoV() const
{
    return fov_;
}
ProjectionType PerspectiveProjection::GetType() const
{
    return ProjectionType::Perspective;
}
std::unique_ptr<IProjection> PerspectiveProjection::Clone() const
{
    return std::make_unique<PerspectiveProjection>(renderingSize_, nearPlane_, farPlane_, fov_);
}
}  // namespace GameEngine
