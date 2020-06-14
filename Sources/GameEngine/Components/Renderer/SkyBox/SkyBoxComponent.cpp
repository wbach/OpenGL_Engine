#include "SkyBoxComponent.h"

#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/CubeMapTexture.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SkyBoxComponent::type = ComponentsType::SkyBox;

SkyBoxComponent::SkyBoxComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::SkyBox, componentContext, gameObject)
    , isSubscribed_(false)
{
    dayTextureFiles_ = {"right.bmp", "left.bmp", "top.bmp", "bottom.bmp", "back.bmp", "front.bmp"};
    nightTextureFiles_ = { "right.bmp", "left.bmp", "top.bmp", "bottom.bmp", "back.bmp", "front.bmp" };
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
void SkyBoxComponent::DeleteTexture(CubeMapTexture*& texture)
{
    if (texture)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*texture);
    }
    texture = nullptr;
}
SkyBoxComponent& SkyBoxComponent::SetDayTexture(const std::array<File, 6>& filenames)
{
    dayTextureFiles_ = filenames;
    TextureParameters params;
    params.flipMode = TextureFlip::VERTICAL;
    dayTexture_     = componentContext_.resourceManager_.GetTextureLoader().LoadCubeMap(filenames, params);
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetNightTexture(const std::array<File, 6>& filenames)
{
    nightTextureFiles_ = filenames;
    TextureParameters params;
    params.flipMode = TextureFlip::VERTICAL;
    nightTexture_ = componentContext_.resourceManager_.GetTextureLoader().LoadCubeMap(filenames, params);
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
const std::array<File, 6>& SkyBoxComponent::GetDayTextureFiles() const
{
    return dayTextureFiles_;
}
const std::array<File, 6>& SkyBoxComponent::GetNightTextureFiles() const
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
