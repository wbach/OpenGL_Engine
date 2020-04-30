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

TerrainRendererComponent::TerrainRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
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

void TerrainRendererComponent::CleanUp()
{
    terrainComponent_->CleanUp();
}

TerrainRendererComponent& TerrainRendererComponent::LoadTextures(
    const std::unordered_map<TerrainTextureType, std::string>& textures)
{
    terrainComponent_->LoadTextures(textures);
    return *this;
}

const std::unordered_map<TerrainTextureType, std::string>& TerrainRendererComponent::GetTextureFileNames() const
{
    return terrainComponent_->GetTextureFileNames();
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
    for (auto f : terrainComponent_->FunctionsToRegister())
    {
        RegisterFunction(f.first, f.second);
    }
    functionsRegistered_ = true;
}
}  // namespace Components
}  // namespace GameEngine
