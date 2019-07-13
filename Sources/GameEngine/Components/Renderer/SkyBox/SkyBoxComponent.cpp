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
    UnSubscribe();
}
SkyBoxComponent& SkyBoxComponent::SetDayTexture(const std::vector<std::string>& filenames)
{
    dayTextureFiles_ = filenames;
    dayTexture_      = componentContext_.resourceManager_.GetTextureLaoder().LoadCubeMap(filenames);
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetNightTexture(const std::vector<std::string>& filenames)
{
    nightTextureFiles_ = filenames;
    nightTexture_      = componentContext_.resourceManager_.GetTextureLaoder().LoadCubeMap(filenames);
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
const std::vector<std::string>& SkyBoxComponent::GetDayTextureFiles() const
{
    return dayTextureFiles_;
}
const std::vector<std::string>& SkyBoxComponent::GetNightTextureFiles() const
{
    return nightTextureFiles_;
}
const std::string& SkyBoxComponent::GetModelFileName() const
{
    return modelFileName_;
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
