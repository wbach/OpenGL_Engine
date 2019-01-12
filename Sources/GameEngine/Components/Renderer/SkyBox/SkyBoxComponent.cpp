#include "SkyBoxComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SkyBoxComponent::type = ComponentsType::SkyBox;

SkyBoxComponent::SkyBoxComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::SkyBox, componentContext, gameObject)
{
}
SkyBoxComponent::~SkyBoxComponent()
{
}
SkyBoxComponent& SkyBoxComponent::SetDayTexture(const std::vector<std::string>& filenames)
{
    dayTexture_ = componentContext_.resourceManager_.GetTextureLaoder().LoadCubeMap(filenames);
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetNightTexture(const std::vector<std::string>& filenames)
{
    nightTexture_ = componentContext_.resourceManager_.GetTextureLaoder().LoadCubeMap(filenames);
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetModel(const std::string& filename)
{
    model_ = componentContext_.resourceManager_.LoadModel(filename);
    return *this;
}
void SkyBoxComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&SkyBoxComponent::Subscribe, this));
}
Texture* SkyBoxComponent::GetDayTexture()
{
    return dayTexture_;
}
Texture* SkyBoxComponent::GetNightTexture()
{
    return nightTexture_;
}
Model* SkyBoxComponent::GetModel()
{
    return model_;
}
void SkyBoxComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void SkyBoxComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine
