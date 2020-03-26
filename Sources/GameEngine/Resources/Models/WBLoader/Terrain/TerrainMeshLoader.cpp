#include "TerrainMeshLoader.h"
#include <Logger/Log.h>
#include <algorithm>
#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
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
    auto fullFilePath      = EngineConf_GetFullDataPathAddToRequierd(filename);
    auto texture           = textureLoader_.LoadHeightMap(fullFilePath);
    auto terrainConfigFile = Utils::GetPathAndFilenameWithoutExtension(fullFilePath) + ".terrainConfig";
    terrainScale_          = TerrainConfiguration::ReadFromFile(terrainConfigFile).GetScale();

    auto hm              = static_cast<HeightMap*>(texture);
    heightMapResolution_ = hm->GetImage()->width;
    heights_             = &hm->GetImage()->floatData;
    halfMaximumHeight_   = (*std::max_element(heights_->begin(), heights_->end())) / 2.f;
    halfMaximumHeight_ *= terrainScale_.y;

    DEBUG_LOG("halfMaximumHeight_ = " + std::to_string(halfMaximumHeight_));

    model_ = std::make_unique<Model>();

    Material material;
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(), material);

    ReserveMeshData(newMesh, heightMapResolution_);
    CreateTerrainVertexes(newMesh, 0, 0, heightMapResolution_, heightMapResolution_);
    CreateIndicies(newMesh, static_cast<IndicesDataType>(heightMapResolution_));

    model_->AddMesh(newMesh);
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
    }

    return std::move(model_);
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
    return ((*heights_)[x + y * heightMapResolution_] * terrainScale_.y) - halfMaximumHeight_;
}
void TerrainMeshLoader::CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size)
{
    auto& indices = mesh.GetMeshDataRef().indices_;

    // Triagnle strip
    for (IndicesDataType gz = 0; gz < size - 1; gz++)
    {
        if ((gz & 1) == 0)
        {  // even rows
            for (IndicesDataType gx = 0; gx < size; gx++)
            {
                indices.push_back(gx + gz * size);
                indices.push_back(gx + (gz + 1) * size);
            }
        }
        else
        {  // odd rows
            for (IndicesDataType gx = size - 1; gx > 0; gx--)
            {
                indices.push_back(gx + (gz + 1) * size);
                indices.push_back(gx - 1 + +gz * size);
            }
        }
    }
}
void TerrainMeshLoader::Clear()
{
    heightMapResolution_ = 0;
    heights_->clear();
}
}  // namespace WBLoader
}  // namespace GameEngine
