#include "TerrainMeshLoader.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
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
void TerrainMeshLoader::ParseFile(const File& file)
{
    TextureParameters params;
    params.loadType = TextureLoadType::None;

    auto heightMap = textureLoader_.LoadHeightMap(file, params);

    if (not heightMap)
    {
        ERROR_LOG("Height mapt not loaded. " + file.GetFilename());
        return;
    }

    model_ = createModel(*heightMap);
    textureLoader_.DeleteTexture(*heightMap);
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

std::unique_ptr<Model> TerrainMeshLoader::createModel(const HeightMap& heightMap)
{
    auto terrainConfig =
        TerrainConfiguration::ReadFromFile(heightMap.GetFile()->GetAbsolutePathWithDifferentExtension("terrainConfig"));
    auto terrainScale = terrainConfig.GetScale();

    auto heightMapResolution = heightMap.GetImage().width;

    auto model = std::make_unique<Model>();

    TerrainHeightTools tools(terrainConfig.GetScale(), heightMap.GetImage());

    auto partsCount = terrainConfig.GetPartsCount();
    if (partsCount)
    {
        CreatePartial(*model, tools, *partsCount);
    }
    else
    {
        CreateAsSingleTerrain(*model, tools);
    }
    return model;
}

void TerrainMeshLoader::CreateAsSingleTerrain(Model& model, TerrainHeightTools& tools)
{
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(), Material());
    auto heightMapResolution = tools.getHeightMapResolution();

    ReserveMeshData(newMesh, heightMapResolution);
    CreateTerrainVertexes(tools, newMesh, 0, 0, heightMapResolution, heightMapResolution);
    CreateIndicies(newMesh, static_cast<IndicesDataType>(heightMapResolution));
    model.AddMesh(newMesh);
}

void TerrainMeshLoader::CreatePartial(Model& model, TerrainHeightTools& tools, uint32 partsCount)
{
    auto heightMapResolution = tools.getHeightMapResolution();
    auto partialSize         = heightMapResolution / partsCount;
    auto rest                = heightMapResolution - (partsCount * partialSize);

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
            CreateTerrainVertexes(tools, newMesh, startX, startY, endX, endY);
            CreateIndicies(newMesh, static_cast<IndicesDataType>(partialSize + 1));
            model.AddMesh(newMesh);
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
void TerrainMeshLoader::CreateTerrainVertexes(TerrainHeightTools& tools, GameEngine::Mesh& mesh, uint32 x_start,
                                              uint32 y_start, uint32 width, uint32 height)
{
    auto& vertices           = mesh.GetMeshDataRef().positions_;
    auto& normals            = mesh.GetMeshDataRef().normals_;
    auto& textureCoords      = mesh.GetMeshDataRef().textCoords_;
    auto& tangents           = mesh.GetMeshDataRef().tangents_;
    auto heightMapResolution = tools.getHeightMapResolution();
    auto terrainScale        = tools.getTerrainScale();

    for (uint32 i = y_start; i < height; i++)
    {
        for (uint32 j = x_start; j < width; j++)
        {
            float height = tools.GetHeight(j, i);

            vertices.push_back(static_cast<float>(j - (terrainScale.x / 2)) /
                               (static_cast<float>(heightMapResolution - 1)) * terrainScale.x);
            vertices.push_back(height);
            vertices.push_back(static_cast<float>(i - (terrainScale.z / 2)) /
                               (static_cast<float>(heightMapResolution) - 1) * terrainScale.z);

            vec3 normal  = tools.GetNormal(j, i);
            vec3 tangnet = tools.GetTangent(normal);

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            tangents.push_back(tangnet.x);
            tangents.push_back(tangnet.y);
            tangents.push_back(tangnet.z);

            textureCoords.push_back(static_cast<float>(j) / static_cast<float>(heightMapResolution - 1));
            textureCoords.push_back(static_cast<float>(i) / static_cast<float>(heightMapResolution - 1));
        }
    }
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
}  // namespace WBLoader
}  // namespace GameEngine
