#include "TerrainRendererComponent.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/ConfigurationParams/RendererParams/TerrainParam/TerrainType.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "TerrainMeshRendererComponent.h"
#include "TerrainTessellationRendererComponent.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string CSTR_TEXTURE_FILENAMES = "textureFileNames";
const std::string CSTR_TEXTURE_TYPE      = "textureType";
const std::string COMPONENT_STR          = "TerrainRenderer";
}  // namespace

TerrainRendererComponent::RendererType Convert(Params::TerrainType type)
{
    switch (type)
    {
        case Params::TerrainType::Mesh:
            return TerrainRendererComponent::RendererType::Mesh;
        case Params::TerrainType::Tessellation:
            return TerrainRendererComponent::RendererType::Tessellation;
    }

    return TerrainRendererComponent::RendererType::Mesh;
}

TerrainRendererComponent::TerrainRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(TerrainRendererComponent).hash_code(), componentContext, gameObject)
    , functionsRegistered_(false)
{
    if (EngineConf.renderer.type == GraphicsApi::RendererType::SIMPLE)
    {
        DEBUG_LOG("Choosed simple renderer force using terrain mesh renderer.");
        SetRendererType(RendererType::Mesh);
    }
    else if (not componentContext_.resourceManager_.GetGraphicsApi().IsTesselationSupported())
    {
        DEBUG_LOG("Tessellation is not supported force using terrain mesh renderer.");
        SetRendererType(RendererType::Mesh);
        return;
    }
    else
    {
        SetRendererType(Convert(EngineConf.renderer.terrain.terrainType));
        SetRendererType(Convert(EngineConf.renderer.terrain.terrainType));
    }
}

TerrainRendererComponent::~TerrainRendererComponent()
{
    DEBUG_LOG("destructor " + std::to_string(type_));
}

void TerrainRendererComponent::CleanUp()
{
    terrainComponent_->CleanUp();
}

TerrainRendererComponent& TerrainRendererComponent::LoadTextures(
    const std::vector<TerrainComponentBase::TerrainTexture>& textures)
{
    terrainComponent_->LoadTextures(textures);
    return *this;
}

Texture* TerrainRendererComponent::GetTexture(TerrainTextureType type) const
{
    return terrainComponent_->GetTexture(type);
}

const std::vector<TerrainComponentBase::TerrainTexture>& TerrainRendererComponent::GetInputDataTextures() const
{
    return terrainComponent_->GetInputDataTextures();
}

void TerrainRendererComponent::UpdateTexture(TerrainTextureType textureType, const std::string& texture)
{
    terrainComponent_->UpdateTexture(textureType, texture);
}

const TerrainConfiguration& TerrainRendererComponent::GetTerrainConfiguration() const
{
    return terrainComponent_->GetConfiguration();
}

void TerrainRendererComponent::SetRendererType(TerrainRendererComponent::RendererType type)
{
    if (terrainComponent_)
    {
        terrainComponent_->CleanUp();
    }

    switch (type)
    {
        case RendererType::Mesh:
            DEBUG_LOG("Set RendererType::Mesh");
            terrainComponent_ = std::make_unique<TerrainMeshRendererComponent>(componentContext_, thisObject_);
            break;
        case RendererType::Tessellation:
            DEBUG_LOG("Set RendererType::Tessellation");
            terrainComponent_ = std::make_unique<TerrainTessellationRendererComponent>(componentContext_, thisObject_);
            break;
    }

    if (functionsRegistered_)
    {
        ReqisterFunctions();
    }
    rendererType_ = type;
}

TerrainRendererComponent::RendererType TerrainRendererComponent::GetRendererType() const
{
    return rendererType_;
}

TerrainTessellationRendererComponent* TerrainRendererComponent::GetTesselationTerrain()
{
    if (rendererType_ != RendererType::Tessellation)
    {
        ERROR_LOG("Get tesselation comonent when rendererType_ != RendererType::Tessellation");
        return nullptr;
    }

    return static_cast<TerrainTessellationRendererComponent*>(terrainComponent_.get());
}

TerrainMeshRendererComponent* TerrainRendererComponent::GetMeshTerrain()
{
    if (rendererType_ != RendererType::Mesh)
    {
        ERROR_LOG("Get tesselation comonent when rendererType_ != RendererType::Mesh");
        return nullptr;
    }

    return static_cast<TerrainMeshRendererComponent*>(terrainComponent_.get());
}

HeightMap* TerrainRendererComponent::GetHeightMap()
{
    return terrainComponent_->GetHeightMap();
}

void TerrainRendererComponent::HeightMapChanged()
{
    terrainComponent_->HeightMapChanged();
}

void TerrainRendererComponent::BlendMapChanged()
{
    terrainComponent_->BlendMapChanged();
}

void TerrainRendererComponent::InitFromParams(const std::unordered_map<std::string, std::string>& params)
{
    for (const auto& param : params)
    {
        if (param.second.empty())
            continue;

        if (IsTerrainTextureType(param.first))
        {
            TerrainTextureType textureType;
            std::from_string(param.first, textureType);
            UpdateTexture(textureType, GetRelativeDataPath(param.second));
        }
        else if (param.first == "redTiledScale")
        {
            float v = Utils::StringToFloat(param.second);
            terrainComponent_->getTerrainTexture(TerrainTextureType::redTexture)->tiledScale             = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::redTextureDisplacement)->tiledScale = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::redTextureNormal)->tiledScale       = v;
        }
        else if (param.first == "blueTiledScale")
        {
            float v = Utils::StringToFloat(param.second);
            terrainComponent_->getTerrainTexture(TerrainTextureType::blueTexture)->tiledScale             = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::blueTextureNormal)->tiledScale       = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::blueTextureDisplacement)->tiledScale = v;
        }
        else if (param.first == "greenTiledScale")
        {
            float v = Utils::StringToFloat(param.second);
            terrainComponent_->getTerrainTexture(TerrainTextureType::greenTexture)->tiledScale             = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::greenTextureNormal)->tiledScale       = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::greenTextureDisplacement)->tiledScale = v;
        }
        else if (param.first == "alphaTiledScale")
        {
            float v = Utils::StringToFloat(param.second);
            terrainComponent_->getTerrainTexture(TerrainTextureType::alphaTexture)->tiledScale             = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::alphaTextureDisplacement)->tiledScale = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::alphaTextureNormal)->tiledScale       = v;
        }
        else if (param.first == "rockTiledScale")
        {
            float v = Utils::StringToFloat(param.second);
            terrainComponent_->getTerrainTexture(TerrainTextureType::rockTexture)->tiledScale             = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::rockTextureNormal)->tiledScale       = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::rockTextureDisplacement)->tiledScale = v;
        }
        else if (param.first == "bgTiledScale")
        {
            float v = Utils::StringToFloat(param.second);
            terrainComponent_->getTerrainTexture(TerrainTextureType::backgorundTexture)->tiledScale             = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::backgorundTextureNormal)->tiledScale       = v;
            terrainComponent_->getTerrainTexture(TerrainTextureType::backgorundTextureDisplacement)->tiledScale = v;
        }
    }
    terrainComponent_->updateTerrainTextureBuffer();
}

std::unordered_map<ParamName, Param> TerrainRendererComponent::GetParams() const
{
    std::unordered_map<ParamName, Param> result;

    const auto& textures = GetInputDataTextures();

    for (const auto& texture : textures)
    {
        auto varType = texture.type == TerrainTextureType::heightmap ? FILE : IMAGE_FILE;
        result.insert({std::to_string(texture.type), {varType, texture.file.GetAbsoultePath()}});
    }

    {
        auto texture = terrainComponent_->getTerrainTexture(TerrainTextureType::redTexture);
        result.insert({"redTiledScale", {FLOAT, std::to_string(texture ? texture->tiledScale : 1.f)}});
    }
    {
        auto texture = terrainComponent_->getTerrainTexture(TerrainTextureType::blueTexture);
        result.insert({"blueTiledScale", {FLOAT, std::to_string(texture ? texture->tiledScale : 1.f)}});
    }
    {
        auto texture = terrainComponent_->getTerrainTexture(TerrainTextureType::greenTexture);
        result.insert({"greenTiledScale", {FLOAT, std::to_string(texture ? texture->tiledScale : 1.f)}});
    }
    {
        auto texture = terrainComponent_->getTerrainTexture(TerrainTextureType::alphaTexture);
        result.insert({"alphaTiledScale", {FLOAT, std::to_string(texture ? texture->tiledScale : 1.f)}});
    }
    {
        auto texture = terrainComponent_->getTerrainTexture(TerrainTextureType::rockTexture);
        result.insert({"rockTiledScale", {FLOAT, std::to_string(texture ? texture->tiledScale : 1.f)}});
    }
    {
        auto texture = terrainComponent_->getTerrainTexture(TerrainTextureType::backgorundTexture);
        result.insert({"bgTiledScale", {FLOAT, std::to_string(texture ? texture->tiledScale : 1.f)}});
    }
    return result;
}

void TerrainRendererComponent::RecalculateNormals()
{
    terrainComponent_->RecalculateNormals();
}

void TerrainRendererComponent::ReqisterFunctions()
{
    const auto functions = terrainComponent_->FunctionsToRegister();
    for (auto f : functions)
    {
        RegisterFunction(f.first, f.second);
    }
    functionsRegistered_ = true;
}

std::vector<Components::TerrainComponentBase::TerrainTexture> ReadTerrainTextures(const TreeNode& node)
{
    std::vector<Components::TerrainComponentBase::TerrainTexture> result;

    for (const auto& texture : node.getChildren())
    {
        Components::TerrainComponentBase::TerrainTexture terrainTexture;
        if (texture->getChild(CSTR_TEXTURE_FILENAME))
            terrainTexture.file = File(texture->getChild(CSTR_TEXTURE_FILENAME)->value_);

        if (texture->getChild(CSTR_TEXTURE_TYPE))
            std::from_string(texture->getChild(CSTR_TEXTURE_TYPE)->value_, terrainTexture.type);
        else
            WARNING_LOG("Read texture without type");

        Read(texture->getChild(CSTR_SCALE), terrainTexture.tiledScale);

        result.push_back(terrainTexture);
    }
    std::sort(result.begin(), result.end(),
              [](const auto& l, const auto& r) { return static_cast<int>(l.type) < static_cast<int>(r.type); });
    return result;
}
void TerrainRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<TerrainRendererComponent>(componentContext, gameObject);

        auto texturesNode = node.getChild(CSTR_TEXTURE_FILENAMES);
        if (texturesNode)
        {
            auto textures = ReadTerrainTextures(*node.getChild(CSTR_TEXTURE_FILENAMES));
            component->LoadTextures(textures);
        }
        else
        {
            ERROR_LOG("Child node with textures not found in terrain render component.");
        }
        return component;
    };

    ReadFunctions::instance().componentsReadFunctions.insert({COMPONENT_STR, readFunc});
}

namespace
{
void create(TreeNode& node, TerrainTextureType type, float scale, const std::string& filename)
{
    ::write(node.addChild(CSTR_TEXTURE_TYPE), std::to_string(type));
    ::write(node.addChild(CSTR_SCALE), std::to_string(scale));
    ::write(node.addChild(CSTR_TEXTURE_FILENAME), filename);
}

void create(TreeNode& node, const std::vector<Components::TerrainComponentBase::TerrainTexture>& textures)
{
    for (const auto& value : textures)
    {
        create(node.addChild(CSTR_TEXTURE), value.type, value.tiledScale, value.file.GetDataRelativeDir());
    }
}
}  // namespace

void TerrainRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    create(node.addChild(CSTR_TEXTURE_FILENAMES), GetInputDataTextures());

    auto heightMapTexture = GetTexture(TerrainTextureType::heightmap);

    if (heightMapTexture and heightMapTexture->IsModified())
    {
        auto heightMap = static_cast<HeightMap*>(heightMapTexture);
        if (heightMap and heightMap->GetFile())
        {
            Utils::CreateBackupFile(heightMap->GetFile()->GetAbsoultePath());
            SaveHeightMap(*heightMap, heightMap->GetFile()->GetAbsoultePath());
        }
        else
        {
            ERROR_LOG("Heightmap texture cast error");
        }
    }

    auto blendMapTexture = GetTexture(TerrainTextureType::blendMap);

    if (blendMapTexture)
    {
        if (blendMapTexture->IsModified() and blendMapTexture->GetFile())
        {
            auto blendMap     = static_cast<GeneralTexture*>(blendMapTexture);
            const auto& image = blendMap->GetImage();
            Utils::CreateBackupFile(blendMapTexture->GetFile()->GetAbsoultePath());

            std::visit(visitor{
                           [&](const std::vector<uint8>& data) {
                               Utils::SaveImage(data, image.size(), blendMapTexture->GetFile()->GetAbsoultePath());
                           },
                           [](const std::vector<float>& data) { DEBUG_LOG("Float version not implemented."); },
                           [](const std::monostate&) { ERROR_LOG("Image data is not set!"); },
                       },
                       image.getImageData());
        }
    }
}

}  // namespace Components
}  // namespace GameEngine
