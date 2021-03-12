#include "SkyBoxComponent.h"

#include <Utils/TreeNode.h>

#include <array>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/CubeMapTexture.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string CSTR_COMPONENT_SKYBOX = "SkyBox";
const std::string CSTR_DAY_TEXTURES     = "dayTextures";
const std::string CSTR_NIGHT_TEXTURES   = "nightTextures";
}  // namespace

std::array<File, 6> ReadCubeMapArray(const TreeNode& node, const std::string& str)
{
    std::array<File, 6> textures;
    uint32 index = 0;
    for (const auto& modelFileName : node.getChild(str)->getChildren())
    {
        if (index < 6)
        {
            textures[index++] = modelFileName->value_;
        }
        else
        {
            ERROR_LOG("To many textures in cubeMap texture.");
        }
    }
    return textures;
}
void Create(TreeNode& node, const std::array<File, 6>& str)
{
    const std::string CSTR_TEXTURE_FILENAME = "textureFileName";
    for (const auto& value : str)
    {
        node.addChild(CSTR_TEXTURE_FILENAME, value.GetDataRelativeDir());
    }
}
void read(const TreeNode& node, Components::SkyBoxComponent& component)
{
    component.SetDayTexture(ReadCubeMapArray(node, CSTR_DAY_TEXTURES));
    component.SetNightTexture(ReadCubeMapArray(node, CSTR_NIGHT_TEXTURES));
    component.SetModel(node.getChild(CSTR_MODEL_FILE_NAME)->value_);
}

SkyBoxComponent::SkyBoxComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(SkyBoxComponent).hash_code(), componentContext, gameObject)
    , isSubscribed_(false)
{
    dayTextureFiles_   = {"right.bmp", "left.bmp", "top.bmp", "bottom.bmp", "back.bmp", "front.bmp"};
    nightTextureFiles_ = {"right.bmp", "left.bmp", "top.bmp", "bottom.bmp", "back.bmp", "front.bmp"};
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
    nightTexture_   = componentContext_.resourceManager_.GetTextureLoader().LoadCubeMap(filenames, params);
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

void SkyBoxComponent::registerReadFunctions()
{
    ReadFunctions::instance().componentsReadFunctions.insert(
        {CSTR_COMPONENT_SKYBOX,
         [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
             auto component = std::make_unique<SkyBoxComponent>(componentContext, gameObject);
             read(node, *component);
             return component;
         }});
}
void SkyBoxComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, CSTR_COMPONENT_SKYBOX});
    Create(node.addChild(CSTR_DAY_TEXTURES), dayTextureFiles_);
    Create(node.addChild(CSTR_NIGHT_TEXTURES), nightTextureFiles_);
    node.addChild(CSTR_MODEL_FILE_NAME, GetModelFileName());
}
}  // namespace Components
}  // namespace GameEngine
