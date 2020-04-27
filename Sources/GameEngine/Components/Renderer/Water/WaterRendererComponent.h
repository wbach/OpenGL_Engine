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
    ~WaterRendererComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;

    float moveFactor() const;
    float increaseAndGetMoveFactor(float deltaTime);

    const vec3& GetPosition() const;
    const vec3& GetScale() const;
    const vec4& GetWaterColor() const;
    float GetWaveSpeed() const;
    const Texture* GetDudvTexture() const;
    const Texture* GetNormalTexture() const;

    void SetPosition(const vec3&);
    void SetScale(const vec3&);
    void SetWaterColor(const vec4&);
    WaterRendererComponent& SetWaveSpeed(float);
    WaterRendererComponent& LoadTextures(const std::string& dudv, const std::string& normal);

private:
    void Subscribe();
    void UnSubscribe();

private:
    void DeleteTextures();

private:
    vec3 position_;
    vec3 scale_;
    float moveFactor_;
    vec4 waterColor_;
    float waveSpeed_;

    Texture* normalMap_;
    Texture* dudvMap_;

    bool isSubscribed_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
