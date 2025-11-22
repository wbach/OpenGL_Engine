#pragma once
#include <Types.h>

namespace GameEngine
{
class IProjection
{
public:
    virtual ~IProjection() = default;

    virtual void UpdateMatrix()                = 0;
    virtual const glm::mat4& GetMatrix() const = 0;

    virtual float GetNear() const                  = 0;
    virtual float GetFar() const                   = 0;
    virtual const vec2ui& GetRenderingSize() const = 0;
    virtual float GetViewDistance() const          = 0;
    virtual float GetAspectRatio() const           = 0;
    virtual vec4 GetBufferParams() const = 0;
};
}  // namespace GameEngine
