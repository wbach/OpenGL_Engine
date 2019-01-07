#include "SkyBoxComponent.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SkyBoxComponent::type = ComponentsType::SkyBox;

SkyBoxComponent::SkyBoxComponent()
    : BaseComponent(ComponentsType::SkyBox)
{
}
SkyBoxComponent::~SkyBoxComponent()
{
}
void SkyBoxComponent::SetDayTexture(const std::vector<std::string>& filenames)
{
    dayTexture_ = resourceManager_->GetTextureLaoder().LoadCubeMap(filenames);
}
void SkyBoxComponent::SetNightTexture(const std::vector<std::string>& filenames)
{
    nightTexture_ = resourceManager_->GetTextureLaoder().LoadCubeMap(filenames);
}
void SkyBoxComponent::SetModel(const std::string & filename)
{
    model_ = resourceManager_->LoadModel(filename);
}
void SkyBoxComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&SkyBoxComponent::Subscribe, this));
}
Texture * SkyBoxComponent::GetDayTexture()
{
    return dayTexture_;
}
Texture * SkyBoxComponent::GetNightTexture()
{
    return nightTexture_;
}
Model * SkyBoxComponent::GetModel()
{
    return model_;
}
void SkyBoxComponent::Subscribe()
{
    if (renderersManager_ == nullptr)
        return;

    renderersManager_->Subscribe(thisObject);
}
void SkyBoxComponent::UnSubscribe()
{
    if (renderersManager_ == nullptr)
        return;

    renderersManager_->UnSubscribe(thisObject);
}
}  // namespace Components
}  // namespace GameEngine
