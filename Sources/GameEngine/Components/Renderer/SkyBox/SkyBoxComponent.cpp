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

std::vector<File> ReadCubeMapArray(const TreeNode& node, const std::string& str)
{
    std::vector<File> textures{"right.bmp", "left.bmp", "top.bmp", "bottom.bmp", "back.bmp", "front.bmp"};
    uint32 index   = 0;
    auto childNode = node.getChild(str);

    if (childNode)
    {
        for (const auto& modelFileName : childNode->getChildren())
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
    }

    return textures;
}

std::optional<std::array<File, 6>> Convert(const std::vector<File>& files)
{
    if (files.size() != 6)
    {
        ERROR_LOG("File size != 6. Current size = " + std::to_string(files.size()));
        return std::nullopt;
    }

    std::array<File, 6> result;
    for (size_t i = 0; i < files.size(); i++)
    {
        result[i] = files[i];
    }
    return result;
}

std::vector<File> Convert(const std::array<File, 6>& files)
{
    std::vector<File> result;
    std::copy(files.cbegin(), files.cend(), std::back_inserter(result));
    return result;
}

void Create(TreeNode& node, const std::vector<File>& str)
{
    const std::string CSTR_TEXTURE_FILENAME = "textureFileName";
    for (const auto& value : str)
    {
        node.addChild(CSTR_TEXTURE_FILENAME, value.GetDataRelativeDir());
    }
}
void read(const TreeNode& node, Components::SkyBoxComponent& component)
{
    component.dayTextureFiles   = ReadCubeMapArray(node, CSTR_DAY_TEXTURES);
    component.nightTextureFiles = ReadCubeMapArray(node, CSTR_NIGHT_TEXTURES);

    auto modelNode = node.getChild(CSTR_MODEL_FILE_NAME);

    if (modelNode)
        component.SetModel(modelNode->value_);
}

SkyBoxComponent::SkyBoxComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(SkyBoxComponent).hash_code(), componentContext, gameObject)
    , modelFile("Meshes/SkyBox/cube.obj")
    , dayTexture_{nullptr}
    , nightTexture_{nullptr}
    , model_{nullptr}
    , isSubscribed_(false)
{
}
void SkyBoxComponent::CleanUp()
{
    UnSubscribe();

    if (model_)
    {
        componentContext_.resourceManager_.ReleaseModel(*model_);
        model_ = nullptr;
    }
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

void SkyBoxComponent::Init()
{
    LoadTextures();
    if (not model_)
    {
        model_ = componentContext_.resourceManager_.LoadModel(modelFile);
    }
    Subscribe();
}
SkyBoxComponent& SkyBoxComponent::SetDayTexture(const std::vector<File>& filenames)
{
    dayTextureFiles = filenames;
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetNightTexture(const std::vector<File>& filenames)
{
    nightTextureFiles = filenames;
    return *this;
}

SkyBoxComponent& SkyBoxComponent::SetDayTexture(const std::array<File, 6>& arrayInput)
{
    dayTextureFiles = Convert(arrayInput);
    return *this;
}

SkyBoxComponent& SkyBoxComponent::SetNightTexture(const std::array<File, 6>& arrayInput)
{
    nightTextureFiles = Convert(arrayInput);
    return *this;
}
SkyBoxComponent& SkyBoxComponent::SetModel(const std::string& filename)
{
    modelFile = filename;
    return *this;
}
void SkyBoxComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&SkyBoxComponent::Init, this));
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
    return modelFile.GetDataRelativeDir();
}
void SkyBoxComponent::Subscribe()
{
    DEBUG_LOG("Try Subscribe");
    if (not isSubscribed_ and (dayTexture_ or nightTexture_))
    {    DEBUG_LOG("Subscribe");
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
     DEBUG_LOG("Subscribe  done");
}
void SkyBoxComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}

void SkyBoxComponent::LoadTextures()
{
    DEBUG_LOG("Load textures");
    TextureParameters params;
    params.flipMode = TextureFlip::VERTICAL;
    auto& loader = componentContext_.resourceManager_.GetTextureLoader();
    if (auto textures = Convert(dayTextureFiles))
    {
        dayTexture_ = loader.LoadCubeMap(*textures, params);
    }
    if (auto textures = Convert(nightTextureFiles))
    {
        nightTexture_ = loader.LoadCubeMap(*textures, params);
    }
    DEBUG_LOG("Load textures done");
}

void SkyBoxComponent::registerReadFunctions()
{
    ReadFunctions::instance().componentsReadFunctions.insert(
        {CSTR_COMPONENT_SKYBOX, [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
         {
             auto component = std::make_unique<SkyBoxComponent>(componentContext, gameObject);
             read(node, *component);
             return component;
         }});
}
void SkyBoxComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, CSTR_COMPONENT_SKYBOX});
    Create(node.addChild(CSTR_DAY_TEXTURES), dayTextureFiles);
    Create(node.addChild(CSTR_NIGHT_TEXTURES), nightTextureFiles);
    node.addChild(CSTR_MODEL_FILE_NAME, GetModelFileName());
}
}  // namespace Components
}  // namespace GameEngine
