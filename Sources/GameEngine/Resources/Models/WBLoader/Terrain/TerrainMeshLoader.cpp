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
bool TerrainMeshLoader::CheckExtension(const File& file)
{
    return file.IsExtension("terrain");
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
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(),
                             GraphicsApi::MeshRawData(), Material());
    auto heightMapResolution = tools.getHeightMapResolution();

    ReserveMeshData(newMesh, heightMapResolution);
    CreateTerrainVertexes(tools, newMesh, 0, 0, heightMapResolution, heightMapResolution);
    CreateIndicies(newMesh, static_cast<IndicesDataType>(heightMapResolution));
    model.setBoundingBox(newMesh.getBoundingBox());
    model.AddMesh(newMesh);
}

void TerrainMeshLoader::CreatePartial(Model& model, TerrainHeightTools& tools, uint32 partsCount)
{
    auto heightMapResolution = tools.getHeightMapResolution();
    auto partialSize         = heightMapResolution / partsCount;
    auto rest                = heightMapResolution - (partsCount * partialSize);

    DEBUG_LOG("Rest : " + std::to_string(rest));

    auto halfTerrainScale = tools.getTerrainScale() * .5f;
    vec3 modelBoundingBoxMin(-halfTerrainScale.x, std::numeric_limits<float>::max(), -halfTerrainScale.z);
    vec3 modelBoundingBoxMax(halfTerrainScale.x, -std::numeric_limits<float>::max(), halfTerrainScale.z);

    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi());

            uint32 startX = i * partialSize;
            uint32 startY = j * partialSize;
            uint32 endX   = (i + 1) * partialSize + 1;
            uint32 endY   = (j + 1) * partialSize + 1;

            ReserveMeshData(newMesh, partialSize + 1);
            CreateTerrainVertexes(tools, newMesh, startX, startY, endX, endY);
            CreateIndicies(newMesh, static_cast<IndicesDataType>(partialSize + 1));
            model.AddMesh(newMesh);

            auto boundingBox = newMesh.getBoundingBox();

            if (boundingBox.min().y < modelBoundingBoxMin.y)
                modelBoundingBoxMin.y = boundingBox.min().y;
            if (boundingBox.max().y > modelBoundingBoxMax.y)
                modelBoundingBoxMax.y = boundingBox.max().y;
        }
    }

    model.setBoundingBox(BoundingBox(modelBoundingBoxMin, modelBoundingBoxMax));
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
    auto heightMapResolution = static_cast<float>(tools.getHeightMapResolution() - 1);
    auto terrainScale        = tools.getTerrainScale();
    auto halfTerrainScale    = terrainScale * .5f;
    auto gridSize            = terrainScale / heightMapResolution;

    float maxHeight = -std::numeric_limits<float>::max();
    float minHeight = std::numeric_limits<float>::max();

    for (uint32 i = y_start; i < height; i++)
    {
        for (uint32 j = x_start; j < width; j++)
        {
            float height = tools.GetHeight(j, i);

            float fj = static_cast<float>(j);
            float fi = static_cast<float>(i);

            auto x = -halfTerrainScale.x + (gridSize.x * fj);
            auto z = -halfTerrainScale.z + (gridSize.z * fi);

            vertices.push_back(x);
            vertices.push_back(height);
            vertices.push_back(z);

            vec3 normal  = tools.GetNormal(j, i);
            vec3 tangnet = tools.GetTangent(normal);

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            tangents.push_back(tangnet.x);
            tangents.push_back(tangnet.y);
            tangents.push_back(tangnet.z);

            textureCoords.push_back(fj / heightMapResolution);
            textureCoords.push_back(fi / heightMapResolution);

            if (height < minHeight)
                minHeight = height;
            if (height > maxHeight)
                maxHeight = height;
        }
    }

    BoundingBox boundingBox;

    auto minX = -halfTerrainScale.x + (gridSize.x * static_cast<float>(x_start));
    auto minZ = -halfTerrainScale.z + (gridSize.z * static_cast<float>(y_start));
    auto maxX = -halfTerrainScale.x + (gridSize.x * static_cast<float>(width));
    auto maxZ = -halfTerrainScale.z + (gridSize.z * static_cast<float>(height));

    boundingBox.minMax(vec3(minX, minHeight, minZ), vec3(maxX, maxHeight, maxZ));
    mesh.setBoundingBox(boundingBox);
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

void TerrainMeshLoader::calcualteAabb(Model& model) const
{
}
}  // namespace WBLoader
}  // namespace GameEngine
