#include "SkyBoxComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SkyBoxComponent::type = ComponentsType::SkyBox;

SkyBoxComponent::SkyBoxComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::SkyBox, componentContext, gameObject)
    , isSubscribed_(false)
{
}
SkyBoxComponent::~SkyBoxComponent()
{
}
void SkyBoxComponent::CleanUp()
{
    UnSubscribe();
    DeleteTexture(dayTexture_);
    DeleteTexture(nightTexture_);
}
void SkyBoxComponent::DeleteTexture(Texture*& texture)
{
    if (texture)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*texture);
    }
    texture = nullptr;
}
SkyBoxComponent& SkyBoxComponent::SetDayTexture(const std::array<std::string, 6>& filenames)
{
    dayTextureFiles_ = filenames;
    dayTexture_ = componentContext_.resourceManager_.GetTextureLoader().LoadCubeMap(filenames, TextureParameters());
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetNightTexture(const std::array<std::string, 6>& filenames)
{
    nightTextureFiles_ = filenames;
    nightTexture_ = componentContext_.resourceManager_.GetTextureLoader().LoadCubeMap(filenames, TextureParameters());
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetModel(const std::string& filename)
{
    modelFileName_ = filename;
    model_         = componentContext_.resourceManager_.LoadModel(filename);
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
const std::array<std::string, 6>& SkyBoxComponent::GetDayTextureFiles() const
{
    return dayTextureFiles_;
}
const std::array<std::string, 6>& SkyBoxComponent::GetNightTextureFiles() const
{
    return nightTextureFiles_;
}
const std::string& SkyBoxComponent::GetModelFileName() const
{
    return modelFileName_;
}
void SkyBoxComponent::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void SkyBoxComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
}  // namespace Components
}  // namespace GameEngine
