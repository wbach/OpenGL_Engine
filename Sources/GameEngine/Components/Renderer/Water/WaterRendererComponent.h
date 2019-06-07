#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
namespace Components
{
class WaterRendererComponent : public BaseComponent
{
public:
    WaterRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);

    const vec3& GetPosition() const;
    const vec3& GetScale() const;
    const vec4& GetWaterColor() const;
    float moveFactor() const;
    float increaseAndGetMoveFactor(float deltaTime);
    Texture* GetDudvTexture();
    Texture* GetNormalTexture();
    WaterRendererComponent& LoadTextures(const std::string& dudv, const std::string& normal);
    WaterRendererComponent& SetWaveSpeed(float);

private:
    void ReqisterFunctions();
    void Subscribe();
    void UnSubscribe();

private:
    vec3 position_;
    vec3 scale_;
    float moveFactor_;
    vec4 waterColor_;
    float waveSpeed_;

    Texture* normalMap_;
    Texture* dudvMap_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
