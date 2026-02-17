#pragma once
#include <Types.h>

#include <memory>

namespace GameEngine
{
enum class ProjectionType
{
    Perspective,
    Orthographic
};

class IProjection
{
public:
    virtual ~IProjection() = default;

    virtual void UpdateMatrix()                = 0;
    virtual const glm::mat4& GetMatrix() const = 0;

    virtual void SetFar(float)                         = 0;
    virtual float GetNear() const                      = 0;
    virtual float GetFar() const                       = 0;
    virtual const vec2ui& GetRenderingSize() const     = 0;
    virtual float GetViewDistance() const              = 0;
    virtual float GetAspectRatio() const               = 0;
    virtual vec4 GetBufferParams() const               = 0;
    virtual void SubscribeForGlobalConfigChange()      = 0;
    virtual ProjectionType GetType() const             = 0;
    virtual std::unique_ptr<IProjection> Clone() const = 0;
};
}  // namespace GameEngine
