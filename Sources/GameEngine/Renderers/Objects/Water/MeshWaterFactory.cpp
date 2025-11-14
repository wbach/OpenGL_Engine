#include "MeshWaterFactory.h"

#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
std::unique_ptr<Model> MeshWaterFactory::CreateAsSingleTile(GraphicsApi::IGraphicsApi& graphicsApi, uint32 resolution)
{
    auto model = std::make_unique<Model>();
    GameEngine::Mesh newMesh(GraphicsApi::RenderType::TRIAGNLE_STRIP, graphicsApi);

    ReserveMeshData(newMesh, resolution);
    CreateVertexes(newMesh, 0, 0, resolution, resolution);
    CreateIndicies(newMesh, static_cast<IndicesDataType>(resolution));
    newMesh.updateBoundingBox();
    model->AddMesh(std::move(newMesh));
    return model;
}
void MeshWaterFactory::ReserveMeshData(GameEngine::Mesh& mesh, uint32 size)
{
    auto& data = mesh.GetMeshDataRef();
    data.positions_.reserve(size * size * 3);
    data.normals_.reserve(size * size * 3);
    data.textCoords_.reserve(size * size * 2);
    data.indices_.reserve(size * size * 2);
}
void MeshWaterFactory::CreateVertexes(GameEngine::Mesh& mesh, uint32 x_start, uint32 y_start, uint32 width, uint32 height)
{
    auto& vertices      = mesh.GetMeshDataRef().positions_;
    auto& normals       = mesh.GetMeshDataRef().normals_;
    auto& textureCoords = mesh.GetMeshDataRef().textCoords_;
    auto& tangents      = mesh.GetMeshDataRef().tangents_;
    auto resolution     = static_cast<float>(height - 1);

    auto gridSize = vec3(1.f) / (resolution);

    for (uint32 i = y_start; i < height; i++)
    {
        for (uint32 j = x_start; j < width; j++)
        {
            float fj = static_cast<float>(j);
            float fi = static_cast<float>(i);

            auto x = -0.5f + (gridSize.x * fj);
            auto z = -0.5f + (gridSize.z * fi);

            float height = 0.f;
            vertices.push_back(x);
            vertices.push_back(height);
            vertices.push_back(z);

            vec3 normal(0, 1, 0);
            vec3 tangnet(1.0, 0.0, 0.0);

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            tangents.push_back(tangnet.x);
            tangents.push_back(tangnet.y);
            tangents.push_back(tangnet.z);

            textureCoords.push_back(fj / resolution);
            textureCoords.push_back(fi / resolution);
        }
    }

    BoundingBox boundingBox;

    auto minX = -0.5f + (gridSize.x * static_cast<float>(x_start));
    auto minZ = -0.5f + (gridSize.z * static_cast<float>(y_start));
    auto maxX = -0.5f + (gridSize.x * static_cast<float>(width));
    auto maxZ = -0.5f + (gridSize.z * static_cast<float>(height));

    // Based is 0, but waves could go up and down
    float maxHeight = -std::numeric_limits<float>::max();
    float minHeight = std::numeric_limits<float>::max();

    boundingBox.minMax(vec3(minX, minHeight, minZ), vec3(maxX, maxHeight, maxZ));
    mesh.setBoundingBox(boundingBox);
}
void MeshWaterFactory::CreateIndicies(GameEngine::Mesh& mesh, IndicesDataType size)
{
    auto& indices = mesh.GetMeshDataRef().indices_;

    if (size < 2)
        return;  // musimy mieć przynajmniej 2 wiersze i 2 kolumny

    // Triangle strip (snake pattern)
    for (IndicesDataType row = 0; row < size - 1; ++row)
    {
        if ((row & 1) == 0)
        {  // parzyste wiersze: lewo -> prawo
            for (IndicesDataType col = 0; col < size; ++col)
            {
                indices.push_back(col + row * size);
                indices.push_back(col + (row + 1) * size);
            }
        }
        else
        {                                                 // nieparzyste wiersze: prawo -> lewo
            for (IndicesDataType col = size; col-- > 0;)  // bezpieczne dla unsigned
            {
                indices.push_back(col + (row + 1) * size);
                indices.push_back(col + row * size);
            }
        }

        // Opcjonalnie: degenerate triangles między wierszami (jeśli strip ma kontynuować dalej)
        if (row < size - 2)
        {
            auto lastIndex      = indices.back();
            auto nextFirstIndex = (row & 1) == 0 ? (size - 1) + (row + 1) * size : 0 + (row + 1) * size;
            indices.push_back(lastIndex);
            indices.push_back(nextFirstIndex);
        }
    }
}
}  // namespace GameEngine