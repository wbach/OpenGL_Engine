#include "TerrainMeshLoader.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace WBLoader
{
TerrainMeshLoader::TerrainMeshLoader(ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
}
void TerrainMeshLoader::ParseFile(const std::string& filename)
{
    auto fullFilePath = EngineConf_GetFullDataPathAddToRequierd(filename);

    TextureParameters params;
    params.loadType = TextureLoadType::None;

    auto texture = textureLoader_.LoadHeightMap(fullFilePath, params);

    if (not texture)
    {
        ERROR_LOG("Height mapt not loaded. " + filename);
        return;
    }

    auto terrainConfigFile = Utils::GetPathAndFilenameWithoutExtension(fullFilePath) + ".terrainConfig";
    auto terrainConfig     = TerrainConfiguration::ReadFromFile(terrainConfigFile);
    terrainScale_          = terrainConfig.GetScale();

    auto hm              = static_cast<HeightMap*>(texture);
    heightMapResolution_ = hm->GetImage().width;
    heights_             = &hm->GetImage().floatData;
    halfMaximumHeight_   = hm->GetMaximumHeight() / 2.f * terrainScale_.y;

    model_ = std::make_unique<Model>();

    auto partsCount = terrainConfig.GetPartsCount();
    if (partsCount)
    {
        CreatePartial(*partsCount);
    }
    else
    {
        CreateAsSingleTerrain();
    }

    textureLoader_.DeleteTexture(*texture);
}
bool TerrainMeshLoader::CheckExtension(const std::string& filename)
{
    auto ext = Utils::GetFileExtension(filename);
    return ext == "terrain";
}
std::unique_ptr<Model> TerrainMeshLoader::Create()
{
    if (not model_)
    {
        ERROR_LOG("Model not created. Please parse file first.");
        return nullptr;
    }

    return std::move(model_);
}

void TerrainMeshLoader::CreateAsSingleTerrain()
{
    Material material;
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(), material);

    ReserveMeshData(newMesh, heightMapResolution_);
    CreateTerrainVertexes(newMesh, 0, 0, heightMapResolution_, heightMapResolution_);
    CreateIndicies(newMesh, static_cast<IndicesDataType>(heightMapResolution_));
    model_->AddMesh(newMesh);
}

void TerrainMeshLoader::CreatePartial(uint32 partsCount)
{
    auto partialSize = heightMapResolution_ / partsCount;
    auto rest        = heightMapResolution_ - (partsCount * partialSize);

    DEBUG_LOG("Rest : " + std::to_string(rest));

    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            Material material;
            GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(),
                                     material);

            uint32 startX = i * partialSize;
            uint32 startY = j * partialSize;
            uint32 endX   = (i + 1) * partialSize + 1;
            uint32 endY   = (j + 1) * partialSize + 1;

            ReserveMeshData(newMesh, partialSize + 1);
            CreateTerrainVertexes(newMesh, startX, startY, endX, endY);
            CreateIndicies(newMesh, static_cast<IndicesDataType>(partialSize + 1));

            model_->AddMesh(newMesh);
        }
    }
}

void TerrainMeshLoader::ReserveMeshData(GameEngine::Mesh& mesh, uint32 size)
{
    auto& data = mesh.GetMeshDataRef();
    data.positions_.reserve(size * size * 3);
    data.normals_.reserve(size * size * 3);
    data.textCoords_.reserve(size * size * 2);
    data.indices_.reserve(size * size * 2);
}
void TerrainMeshLoader::CreateTerrainVertexes(GameEngine::Mesh& mesh, uint32 x_start, uint32 y_start, uint32 width,
                                              uint32 height)
{
    auto& vertices      = mesh.GetMeshDataRef().positions_;
    auto& normals       = mesh.GetMeshDataRef().normals_;
    auto& textureCoords = mesh.GetMeshDataRef().textCoords_;

    for (uint32 i = y_start; i < height; i++)
    {
        for (uint32 j = x_start; j < width; j++)
        {
            float height = GetHeight(j, i);

            vertices.push_back(static_cast<float>(j - (terrainScale_.x / 2)) /
                               (static_cast<float>(heightMapResolution_ - 1)) * terrainScale_.x);
            vertices.push_back(height);
            vertices.push_back(static_cast<float>(i - (terrainScale_.z / 2)) /
                               (static_cast<float>(heightMapResolution_) - 1) * terrainScale_.z);

            glm::vec3 normal = CalculateNormalMap(j, i);

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoords.push_back(static_cast<float>(j) / static_cast<float>(heightMapResolution_ - 1));
            textureCoords.push_back(static_cast<float>(i) / static_cast<float>(heightMapResolution_ - 1));
        }
    }
}
vec3 TerrainMeshLoader::CalculateNormalMap(uint32 x, uint32 z)
{
    auto lx = x - 1;
    if (x == 0)
        lx = 0;
    auto rx = x + 1;
    if (rx > heightMapResolution_ - 1)
        rx = heightMapResolution_ - 1;
    auto dz = z - 1;
    if (z == 0)
        dz = 0;
    auto uz = z + 1;
    if (uz > heightMapResolution_ - 1)
        uz = heightMapResolution_ - 1;

    float heightL = GetHeight(lx, z);
    float heightR = GetHeight(rx, z);
    float heightD = GetHeight(x, dz);
    float heightU = GetHeight(x, uz);
    glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);

    return glm::normalize(normal);
}
float TerrainMeshLoader::GetHeight(uint32 x, uint32 y) const
{
    return GetTerrainHeight(*heights_, terrainScale_.y, heightMapResolution_, halfMaximumHeight_, x, y);
}
void TerrainMeshLoader::CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size)
{
    auto& indices = mesh.GetMeshDataRef().indices_;

    // Triagnle strip
    for (IndicesDataType row = 0; row < size - 1; row++)
    {
        if ((row & 1) == 0)
        {  // even rows
            for (IndicesDataType col = 0; col < size; col++)
            {
                indices.push_back(col + row * size);
                indices.push_back(col + (row + 1) * size);
            }
        }
        else
        {  // odd rows
            for (IndicesDataType col = size - 1; col > 0; col--)
            {
                indices.push_back(col + (row + 1) * size);
                indices.push_back(col - 1 + row * size);
            }
        }
    }

    auto row = size - 1;
    auto col = 0;
    indices.push_back(col + row * size);
}
void TerrainMeshLoader::Clear()
{
    heightMapResolution_ = 0;
    heights_->clear();
}
}  // namespace WBLoader
}  // namespace GameEngine
