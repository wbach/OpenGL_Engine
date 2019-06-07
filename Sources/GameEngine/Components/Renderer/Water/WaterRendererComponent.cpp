#include "WaterRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
const uint32 WATER_SCALE = 4000;

ComponentsType WaterRendererComponent::type = ComponentsType::Water;

WaterRendererComponent::WaterRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(WaterRendererComponent::type, componentContext, gameObject)
    , position_(0, -75, 0)
    , scale_(WATER_SCALE, WATER_SCALE, 1)
    , moveFactor_(0)
    , waterColor_(.0f, .27f, .44f, 1.f)
    , waveSpeed_(.1f)
{
}
void WaterRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&WaterRendererComponent::Subscribe, this));
}
const vec3& WaterRendererComponent::GetPosition() const
{
    return position_;
}
const vec3& WaterRendererComponent::GetScale() const
{
    return scale_;
}

const vec4& WaterRendererComponent::GetWaterColor() const
{
    return waterColor_;
}

float WaterRendererComponent::moveFactor() const
{
    return moveFactor_;
}

float WaterRendererComponent::increaseAndGetMoveFactor(float deltaTime)
{
    moveFactor_ += waveSpeed_ * deltaTime;
    moveFactor_ = fmodf(moveFactor_, 1.f);
    return moveFactor_;
}

Texture* WaterRendererComponent::GetDudvTexture()
{
    return dudvMap_;
}

Texture* WaterRendererComponent::GetNormalTexture()
{
    return normalMap_;
}

WaterRendererComponent& WaterRendererComponent::LoadTextures(const std::string& dudv, const std::string& normal)
{
    normalMap_ = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(normal);
    dudvMap_   = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(dudv);

    return *this;
}

WaterRendererComponent& WaterRendererComponent::SetWaveSpeed(float speed)
{
    waveSpeed_ = speed;
    return *this;
}
void WaterRendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void WaterRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine
