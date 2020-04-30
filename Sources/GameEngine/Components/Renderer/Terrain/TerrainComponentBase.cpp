#include "TerrainComponentBase.h"
#include <Logger/Log.h>
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainComponentBase::TerrainComponentBase(ComponentContext &componentContext, GameObject &gameObject)
    : componentContext_(componentContext)
    , thisObject_(gameObject)
    , heightMap_(nullptr)
    , isSubscribed_(false)
{
}
void TerrainComponentBase::CleanUp()
{
    UnSubscribe();
    ReleaseTextures();
}

void TerrainComponentBase::LoadTextures(const std::unordered_map<TerrainTextureType, std::string> &textures)
{
    texturedFileNames_ = textures;

    for (const auto &texturePair : textures)
    {
        if (texturePair.first == TerrainTextureType::heightmap)
        {
            LoadTerrainConfiguration(texturePair.second);
            LoadHeightMap(texturePair.second);
            continue;
        }

        auto texture =
            componentContext_.resourceManager_.GetTextureLoader().LoadTexture(texturePair.second, TextureParameters());
        SetTexture(texturePair.first, texture);
    }
}

const std::unordered_map<TerrainTextureType, std::string> &TerrainComponentBase::GetTextureFileNames() const
{
    return texturedFileNames_;
}

const TerrainTexturesMap &TerrainComponentBase::GetTextures() const
{
    return textures_;
}

Texture *TerrainComponentBase::GetTexture(TerrainTextureType type)
{
    return textures_.count(type) ? textures_.at(type) : nullptr;
}

const TerrainConfiguration &TerrainComponentBase::GetConfiguration() const
{
    return config_;
}

HeightMap *TerrainComponentBase::GetHeightMap()
{
    return heightMap_;
}

void TerrainComponentBase::UpdateTexture(TerrainTextureType type, const std::string &filename)
{
    texturedFileNames_[type] = filename;

    if (type == TerrainTextureType::heightmap)
    {
        UpdateHeightMap(filename);
    }
    else
    {
        auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(filename, TextureParameters());
        if (texture)
        {
            UpdateTexture(type, texture);
        }
    }
}

void TerrainComponentBase::LoadHeightMap(const std::string &filename)
{
    auto fullFilePath = EngineConf_GetFullDataPathAddToRequierd(filename);
    auto texture =
        componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(fullFilePath, heightMapParameters_);

    if (texture)
    {
        SetTexture(TerrainTextureType::heightmap, texture);
        heightMap_ = static_cast<HeightMap *>(texture);
    }
}

void TerrainComponentBase::LoadTerrainConfiguration(const std::string &terrainFileName)
{
    auto terrainFileFullpath = EngineConf_GetFullDataPath(terrainFileName);
    auto terrainConfigFile   = Utils::GetPathAndFilenameWithoutExtension(terrainFileFullpath) + ".terrainConfig";

    config_ = TerrainConfiguration::ReadFromFile(terrainConfigFile);
}

void TerrainComponentBase::SetTexture(TerrainTextureType type, Texture *texture)
{
    if (textures_.count(type))
    {
        ERROR_LOG("Texture type " + std::to_string(type) + " already exist.");
        return;
    }

    textures_.insert({type, texture});
}

void TerrainComponentBase::UpdateTexture(TerrainTextureType type, Texture *texture)
{
    if (textures_.count(type))
    {
        textures_.at(type) = texture;
    }
    else
    {
        SetTexture(type, texture);
    }
}

void TerrainComponentBase::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}

void TerrainComponentBase::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}

void TerrainComponentBase::ReleaseTextures()
{
    for (auto &texture : textures_)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*texture.second);
    }
    textures_.clear();
}
}  // namespace Components
}  // namespace GameEngine
