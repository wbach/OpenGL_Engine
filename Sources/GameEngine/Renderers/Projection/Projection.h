#pragma once
#include <optional>

#include "IProjection.h"
#include "Types.h"

namespace GameEngine
{
class Projection : public IProjection
{
public:
    Projection();
    Projection(const vec2ui& renderingSize);
    Projection(const vec2ui& renderingSize, float near, float far);

    Projection(const Projection& p);
    ~Projection() override;

    Projection& operator=(const Projection& p);

    const glm::mat4& GetMatrix() const override;
    float GetNear() const override;
    float GetFar() const override;
    const vec2ui& GetRenderingSize() const override;
    float GetViewDistance() const override;
    float GetAspectRatio() const override;
    vec4 GetBufferParams() const override;
    void SubscribeForGlobalConfigChange();

protected:
    float CalculateAspectRatio() const;
    void Init();
    void UnsubscribeForEvents();

protected:
    vec2ui renderingSize_;
    float aspectRatio_;
    float nearPlane_;
    float farPlane_;
    mat4 matrix_;

    std::optional<IdType> viewDistanceChangeSubscription_;
    std::optional<IdType> resolutionChangeSubscription_;
};
}  // namespace GameEngine
