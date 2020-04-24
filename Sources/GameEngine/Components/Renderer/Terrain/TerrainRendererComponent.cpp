#include "TerrainRendererComponent.h"

#include <Logger/Log.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/ConfigurationParams/TerrainParam/TerrainType.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "TerrainMeshRendererComponent.h"
#include "TerrainTessellationRendererComponent.h"
#include "Utils/FileSystem/FileSystemUtils.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
const vec3 DEFAULT_SCALE(1);
const TerrainConfiguration DEFAULT_TERRAIN_CONFIG;
std::unordered_map<TerrainTextureType, std::string> defualtEmptyTexturesMap;
}  // namespace

ComponentsType TerrainRendererComponent::type = ComponentsType::TerrainRenderer;

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

TerrainRendererComponent::TerrainRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
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
    }
}

TerrainRendererComponent::~TerrainRendererComponent()
{
}

TerrainRendererComponent& TerrainRendererComponent::LoadTextures(
    const std::unordered_map<TerrainTextureType, std::string>& texutres)
{
    switch (rendererType_)
    {
        case RendererType::Mesh:
            meshComponent_->LoadTextures(texutres);
            return *this;
        case RendererType::Tessellation:
            tesselationComponent_->LoadTextures(texutres);
            return *this;
    }

    return *this;
}

const std::unordered_map<TerrainTextureType, std::string>& TerrainRendererComponent::GetTextureFileNames() const
{
    switch (rendererType_)
    {
        case RendererType::Mesh:
            return meshComponent_->GetTextureFileNames();
        case RendererType::Tessellation:
            return tesselationComponent_->GetTextureFileNames();
    }

    ERROR_LOG("RendererType unkonown, return default.");
    return defualtEmptyTexturesMap;
}

void TerrainRendererComponent::UpdateTexture(TerrainTextureType textureType, const std::string& texture)
{
    switch (rendererType_)
    {
        case RendererType::Mesh:
            meshComponent_->UpdateTexture(textureType, texture);
            break;
        case RendererType::Tessellation:
            tesselationComponent_->UpdateTexture(textureType, texture);
            break;
    }
}

const TerrainConfiguration& TerrainRendererComponent::GetTerrainConfiguration() const
{
    switch (rendererType_)
    {
        case RendererType::Mesh:
            return meshComponent_->GetConfiguration();
        case RendererType::Tessellation:
            return tesselationComponent_->GetConfig();
    }

    ERROR_LOG("RendererType unkonown, return default terrain config.");
    return DEFAULT_TERRAIN_CONFIG;
}

void TerrainRendererComponent::SetRendererType(TerrainRendererComponent::RendererType type)
{
    switch (type)
    {
        case RendererType::Mesh:
            DEBUG_LOG("Set RendererType::Mesh");
            tesselationComponent_.reset();
            meshComponent_ = std::make_unique<TerrainMeshRendererComponent>(componentContext_, thisObject_);
            if (functionsRegistered_)
                meshComponent_->ReqisterFunctions();
            break;
        case RendererType::Tessellation:
            DEBUG_LOG("Set RendererType::Tessellation");
            meshComponent_.reset();
            tesselationComponent_ =
                std::make_unique<TerrainTessellationRendererComponent>(componentContext_, thisObject_);
            if (functionsRegistered_)
                tesselationComponent_->ReqisterFunctions();
            break;
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

    return tesselationComponent_.get();
}

TerrainMeshRendererComponent* TerrainRendererComponent::GetMeshTerrain()
{
    if (rendererType_ != RendererType::Mesh)
    {
        ERROR_LOG("Get tesselation comonent when rendererType_ != RendererType::Mesh");
        return nullptr;
    }

    return meshComponent_.get();
}

const vec3& TerrainRendererComponent::GetScale() const
{
    switch (rendererType_)
    {
        case RendererType::Mesh:
            return meshComponent_->GetConfiguration().GetScale();
        case RendererType::Tessellation:
            return tesselationComponent_->GetScale();
    }

    ERROR_LOG("RendererType unkonown, return default.");
    return DEFAULT_SCALE;
}

void TerrainRendererComponent::InitFromParams(const std::unordered_map<std::string, std::string>& params)
{
    for (const auto& param : params)
    {
        if (param.second.empty())
            continue;

        if (IsTerrainTextureType(param.first))
        {
            auto textureType = CreateFromString(param.first);
            UpdateTexture(textureType, GetRelativeDataPath(param.second));
        }

    }
}

std::unordered_map<ParamName, Param> TerrainRendererComponent::GetParams() const
{
    std::unordered_map<ParamName, Param> result;

    const auto& textures = GetTextureFileNames();

    for (const auto& texture : textures)
    {
        auto varType = texture.first == TerrainTextureType::heightmap ? FILE : IMAGE_FILE;
        result.insert(
            {std::to_string(texture.first), {varType, Utils::GetAbsolutePath(GetFullDataPath(texture.second))}});
    }

    return result;
}

void TerrainRendererComponent::ReqisterFunctions()
{
    switch (rendererType_)
    {
        case RendererType::Mesh:
            meshComponent_->ReqisterFunctions();
            break;
        case RendererType::Tessellation:
            tesselationComponent_->ReqisterFunctions();
            break;
    }

    functionsRegistered_ = true;
}
}  // namespace Components
}  // namespace GameEngine
