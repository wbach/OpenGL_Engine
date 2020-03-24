#include "TerrainMeshLoader.h"
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

    vertices_.reserve(heightMapResolution_ * heightMapResolution_ * 3);
    normals_.reserve(heightMapResolution_ * heightMapResolution_ * 3);
    textureCoords_.reserve(heightMapResolution_ * heightMapResolution_ * 2);
    indices_.reserve(heightMapResolution_ * heightMapResolution_ * 2);

    CreateTerrainVertexes(0, 0, heightMapResolution_, heightMapResolution_);
    CreateMesh();
}
bool TerrainMeshLoader::CheckExtension(const std::string& filename)
{
    auto ext = Utils::GetFileExtension(filename);
    return ext == "terrain";
}
std::unique_ptr<Model> TerrainMeshLoader::Create()
{
    auto model = std::make_unique<Model>();

    Material material;
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(), material);

    auto maxElement = FindMaxY(vertices_);
    TranslateY(vertices_, maxElement / 2.f);

    newMesh.GetMeshDataRef().positions_  = vertices_;
    newMesh.GetMeshDataRef().textCoords_ = textureCoords_;
    newMesh.GetMeshDataRef().normals_    = normals_;
    newMesh.GetMeshDataRef().tangents_   = tangens_;
    newMesh.GetMeshDataRef().indices_    = indices_;
    model->AddMesh(newMesh);

    // Normals stay
    indices_.clear();
    vertices_.clear();
    tangens_.clear();
    textureCoords_.clear();

    return model;
}
void TerrainMeshLoader::CreateTerrainVertexes(uint32 x_start, uint32 y_start, uint32 width, uint32 height)
{
    const auto terrainSize      = heightMapResolution_ - 1;
    const auto& halfTerrainSize = terrainSize / 2.f;
    for (uint32 i = y_start; i < height; i++)
    {
        for (uint32 j = x_start; j < width; j++)
        {
            float height = GetHeight(j, i);
            vertices_.push_back(static_cast<float>(j - halfTerrainSize) /
                                (static_cast<float>(heightMapResolution_ - 1)) * terrainSize);
            vertices_.push_back(height);
            vertices_.push_back(static_cast<float>(i - halfTerrainSize) /
                                (static_cast<float>(heightMapResolution_) - 1) * terrainSize);

            glm::vec3 normal = CalculateNormalMap(j, i);

            normals_.push_back(normal.x);
            normals_.push_back(normal.y);
            normals_.push_back(normal.z);

            textureCoords_.push_back(static_cast<float>(j) / static_cast<float>(heightMapResolution_ - 1));
            textureCoords_.push_back(static_cast<float>(i) / static_cast<float>(heightMapResolution_ - 1));
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
    return (*heights_)[x + y * heightMapResolution_] * terrainScale_.y;
}
void TerrainMeshLoader::CreateMesh()
{
    // Triaagnle strip
    for (uint32 gz = 0; gz < heightMapResolution_ - 1; gz++)
    {
        if ((gz & 1) == 0)
        {  // even rows
            for (uint32 gx = 0; gx < heightMapResolution_; gx++)
            {
                indices_.push_back(gx + gz * heightMapResolution_);
                indices_.push_back(gx + (gz + 1) * heightMapResolution_);
            }
        }
        else
        {  // odd rows
            for (uint32 gx = heightMapResolution_ - 1; gx > 0; gx--)
            {
                indices_.push_back(gx + (gz + 1) * heightMapResolution_);
                indices_.push_back(gx - 1 + +gz * heightMapResolution_);
            }
        }
    }
}
void TerrainMeshLoader::Clear()
{
    heightMapResolution_ = 0;
    indices_.clear();
    vertices_.clear();
    normals_.clear();
    tangens_.clear();
    textureCoords_.clear();
    heights_->clear();
}
void TerrainMeshLoader::TranslateY(FloatAttributeVec& v, float y)
{
    int x = 0;
    for (auto& v : vertices_)
    {
        if (x == 1)
            v -= y;

        if (x == 2)
            x = 0;
        else
            ++x;
    }
}
float TerrainMeshLoader::FindMaxY(const FloatAttributeVec& v) const
{
    auto maxElement = -std::numeric_limits<float>::max();

    int x = 0;
    for (auto& v : vertices_)
    {
        if (x == 1)
        {
            if (v > maxElement)
                maxElement = v;
        }

        if (x == 2)
            x = 0;
        else
            ++x;
    }
    return maxElement;
}
}  // namespace WBLoader
}  // namespace GameEngine
