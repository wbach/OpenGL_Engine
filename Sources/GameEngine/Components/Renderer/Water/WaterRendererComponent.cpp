#include "WaterRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Objects/GameObject.h"

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
    , waterColor_(Utils::RGBtoFloat(0.f, 44.f, 82.f), 1.f)
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

const Texture* WaterRendererComponent::GetDudvTexture() const
{
    return dudvMap_;
}

const Texture* WaterRendererComponent::GetNormalTexture() const
{
    return normalMap_;
}

void WaterRendererComponent::SetPosition(const vec3& pos)
{
    position_ = pos;
}

void WaterRendererComponent::SetScale(const vec3& s)
{
    scale_ = s;
}

WaterRendererComponent& WaterRendererComponent::LoadTextures(const std::string& dudv, const std::string& normal)
{
    normalMap_ = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(normal, TextureParameters());
    dudvMap_   = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(dudv, TextureParameters());

    return *this;
}

WaterRendererComponent& WaterRendererComponent::SetWaveSpeed(float speed)
{
    waveSpeed_ = speed;
    return *this;
}

float WaterRendererComponent::GetWaveSpeed() const
{
    return waveSpeed_;
}

void WaterRendererComponent::SetWaterColor(const vec4& color)
{
    waterColor_ = color;
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
