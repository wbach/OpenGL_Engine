#include "TreeGenerate.h"

#include "Logger/Log.h"
#include "Tree.h"
#include "TreeMeshBuilder.h"

void AddDebugCube(const glm::vec3& centerPos, float size, GraphicsApi::MeshRawData& mesh)
{
    float h = size * 0.5f;

    glm::vec3 vertices[8] = {
        centerPos + glm::vec3(-h, -h, -h), // 0
        centerPos + glm::vec3( h, -h, -h), // 1
        centerPos + glm::vec3( h,  h, -h), // 2
        centerPos + glm::vec3(-h,  h, -h), // 3
        centerPos + glm::vec3(-h, -h,  h), // 4
        centerPos + glm::vec3( h, -h,  h), // 5
        centerPos + glm::vec3( h,  h,  h), // 6
        centerPos + glm::vec3(-h,  h,  h)  // 7
    };

    // Kolejność CCW dla każdej ściany
    uint32_t cubeIndices[36] = {
        0, 2, 1, 0, 3, 2, // tył
        4, 5, 6, 4, 6, 7, // przód
        0, 1, 5, 0, 5, 4, // dół
        2, 3, 7, 2, 7, 6, // góra
        0, 4, 7, 0, 7, 3, // lewa
        1, 2, 6, 1, 6, 5  // prawa
    };

    uint32_t indexOffset = static_cast<uint32_t>(mesh.positions_.size() / 3);

    for (int i = 0; i < 8; ++i)
    {
        mesh.positions_.push_back(vertices[i].x);
        mesh.positions_.push_back(vertices[i].y);
        mesh.positions_.push_back(vertices[i].z);

        mesh.textCoords_.push_back(0.0f);
        mesh.textCoords_.push_back(0.0f);

        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(0.0f);
        mesh.normals_.push_back(1.0f);

        mesh.tangents_.push_back(1.0f);
        mesh.tangents_.push_back(0.0f);
        mesh.tangents_.push_back(0.0f);

        mesh.bitangents_.push_back(0.0f);
        mesh.bitangents_.push_back(1.0f);
        mesh.bitangents_.push_back(0.0f);

        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);
    }

    for (int i = 0; i < 36; ++i)
        mesh.indices_.push_back(cubeIndices[i] + indexOffset);
}

GraphicsApi::MeshRawData GameEngine::generateTree(int attractorCount, float crownRadius, float crownHeight,
                                                  float attractionDistance, float killDistance, float segmentLength,
                                                  float trunkOffsetY)
{
    LOG_DEBUG << "Generate tree branches...";
    Tree tree{};
    tree.rootPosition = vec3(0, -3.f, 0);
    tree.prepareAttractors(attractorCount, crownRadius);
    auto status = tree.build();

    if (status and not status->empty())
    {
        LOG_WARN << status.value();
    }

    LOG_DEBUG << "Buildng tree mesh... ("
              << "Branches : " << tree.GetBranches().size() << ")";
    TreeMeshBuilder builder(tree.GetBranches());
    auto result = builder.build();

    // for (auto& pos : tree.GetLeafsPositions())
    // {
    //     AddDebugCube(pos, 1.0f, result);
    // }
    return result;
}
