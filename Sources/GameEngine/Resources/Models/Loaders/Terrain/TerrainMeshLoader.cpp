#include "TerrainMeshLoader.h"

#include <Logger/Log.h>

#include <algorithm>
#include <optional>

#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainMeshLoader::TerrainMeshLoader(ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
}
bool TerrainMeshLoader::ParseFile(const File& file)
{
    TextureParameters params;
    params.loadType  = TextureLoadType::None;
    params.sizeLimit = std::nullopt;

    auto heightMap = textureLoader_.LoadHeightMap(file, params);

    if (not heightMap)
    {
        LOG_ERROR << "Height mapt not loaded. " << file;
        return false;
    }

    auto terrainConfig = TerrainConfiguration::ReadFromFile(file.GetDerivedFileWithExtension("terrainConfig"));
    auto partsCount    = terrainConfig.GetPartsCount();
    model_             = createModel(*heightMap, partsCount);
    textureLoader_.DeleteTexture(*heightMap);
    return true;
}
bool TerrainMeshLoader::CheckExtension(const File& file)
{
    return file.IsFormat("terrain");
}
std::unique_ptr<Model> TerrainMeshLoader::Create()
{
    if (not model_)
    {
        LOG_ERROR << "Model not created. Please parse file first.";
        return nullptr;
    }
    return std::move(model_);
}

std::unique_ptr<Model> TerrainMeshLoader::createModel(const HeightMap& heightMap, std::optional<uint32> partsCount)
{
    auto model = std::make_unique<Model>(graphicsApi_);

    TerrainHeightTools tools(vec3(1.f), heightMap.GetImage());

    uint32 factor = EngineConf.renderer.terrain.resolutionDivideFactor;

    if (partsCount and *partsCount > 1)
    {
        CreatePartial(*model, tools, *partsCount, factor);
    }
    else
    {
        CreateAsSingleTerrain(*model, tools, factor);
    }

    return model;
}

void TerrainMeshLoader::CreateAsSingleTerrain(Model& model, TerrainHeightTools& tools, uint32 resolutionDivideFactor)
{
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi(), GraphicsApi::MeshRawData(),
                             Material());
    auto heightMapResolution = tools.getHeightMapResolution();

    const uint32 vertexCount = (heightMapResolution - 1) / resolutionDivideFactor + 1;

    ReserveMeshData(newMesh, vertexCount);
    CreateTerrainVertexes(tools, newMesh, 0, 0, heightMapResolution, heightMapResolution, resolutionDivideFactor);
    CreateIndicies(newMesh, static_cast<IndicesDataType>(vertexCount));
    model.AddMesh(std::move(newMesh));
}

void TerrainMeshLoader::CreatePartial(Model& model, TerrainHeightTools& tools, uint32 partsCount, uint32 resolutionDivideFactor)
{
    if (partsCount == 0 || resolutionDivideFactor == 0)
    {
        LOG_ERROR << "Invalid terrain parts count or resolution divider.";
        return;
    }

    auto heightMapResolution      = tools.getHeightMapResolution();
    const uint32 intervals        = heightMapResolution - 1;
    const uint32 intervalsPerPart = (intervals / partsCount / resolutionDivideFactor) * resolutionDivideFactor;

    vec3 modelBoundingBoxMin(-0.5f, std::numeric_limits<float>::max(), -0.5f);
    vec3 modelBoundingBoxMax(0.5f, -std::numeric_limits<float>::max(), 0.5f);

    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, textureLoader_.GetGraphicsApi());

            const uint32 startX       = i * intervalsPerPart;
            const uint32 startY       = j * intervalsPerPart;
            const uint32 endX         = (i == partsCount - 1) ? heightMapResolution : startX + intervalsPerPart + 1;
            const uint32 endY         = (j == partsCount - 1) ? heightMapResolution : startY + intervalsPerPart + 1;
            const uint32 vertexCountX = (endX - 1 - startX) / resolutionDivideFactor + 1;
            const uint32 vertexCountY = (endY - 1 - startY) / resolutionDivideFactor + 1;

            ReserveMeshData(newMesh, std::max(vertexCountX, vertexCountY));
            CreateTerrainVertexes(tools, newMesh, startX, startY, endX, endY, resolutionDivideFactor);
            CreateIndicies(newMesh, static_cast<IndicesDataType>(vertexCountX));

            auto boundingBox = newMesh.getBoundingBox();

            if (boundingBox.min().y < modelBoundingBoxMin.y)
                modelBoundingBoxMin.y = boundingBox.min().y;
            if (boundingBox.max().y > modelBoundingBoxMax.y)
                modelBoundingBoxMax.y = boundingBox.max().y;

            model.AddMesh(std::move(newMesh));
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
void TerrainMeshLoader::CreateTerrainVertexes(TerrainHeightTools& tools, GameEngine::Mesh& mesh, uint32 x_start, uint32 y_start,
                                              uint32 width, uint32 height, uint32 resolutionDivideFactor)
{
    auto& vertices           = mesh.GetMeshDataRef().positions_;
    auto& normals            = mesh.GetMeshDataRef().normals_;
    auto& textureCoords      = mesh.GetMeshDataRef().textCoords_;
    auto& tangents           = mesh.GetMeshDataRef().tangents_;
    auto heightMapResolution = static_cast<float>(tools.getHeightMapResolution() - 1);

    auto gridSize   = vec3(1.f) / (heightMapResolution);
    float maxHeight = -std::numeric_limits<float>::max();
    float minHeight = std::numeric_limits<float>::max();

    for (uint32 i = y_start; i < height; i += resolutionDivideFactor)
    {
        for (uint32 j = x_start; j < width; j += resolutionDivideFactor)
        {
            float height = tools.GetHeight(j, i);

            float fj = static_cast<float>(j);
            float fi = static_cast<float>(i);

            auto x = -0.5f + (gridSize.x * fj);
            auto z = -0.5f + (gridSize.z * fi);

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

    auto minX = -0.5f + (gridSize.x * static_cast<float>(x_start));
    auto minZ = -0.5f + (gridSize.z * static_cast<float>(y_start));
    auto maxX = -0.5f + (gridSize.x * static_cast<float>(width));
    auto maxZ = -0.5f + (gridSize.z * static_cast<float>(height));

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
}  // namespace GameEngine
