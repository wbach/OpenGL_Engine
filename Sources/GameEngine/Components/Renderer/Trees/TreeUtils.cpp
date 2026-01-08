#include "TreeUtils.h"

#include "GameEngine/Components/Renderer/Trees/Leaf.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
Model* CreateLeafModel(IResourceManager& resourceManager, GraphicsApi::IGraphicsApi& graphicsApi, const std::vector<Leaf>& leafs,
                       const Material& material)
{
    if (leafs.empty())
    {
        LOG_DEBUG << "Leaf position vector is empty";
        return nullptr;
    }

    LOG_DEBUG << "Create leaf mesh";
    GraphicsApi::MeshRawData data;
    data.positions_.reserve(leafs.size() * 3);
    data.textCoords_.reserve(leafs.size() * 2);
    data.normals_.reserve(leafs.size() * 3);
    data.tangents_.reserve(leafs.size() * 3);

    for (const auto& leaf : leafs)
    {
        data.positions_.push_back(leaf.position.x);
        data.positions_.push_back(leaf.position.y);
        data.positions_.push_back(leaf.position.z);

        data.textCoords_.push_back(leaf.sizeRandomness);
        data.textCoords_.push_back(static_cast<float>(leaf.textureIndex));

        data.normals_.push_back(leaf.direction.x);
        data.normals_.push_back(leaf.direction.y);
        data.normals_.push_back(leaf.direction.z);

        data.tangents_.push_back(leaf.colorRandomness.x);
        data.tangents_.push_back(leaf.colorRandomness.y);
        data.tangents_.push_back(leaf.colorRandomness.z);
    }

    auto model  = std::make_unique<GameEngine::Model>();
    auto result = model.get();
    model->AddMesh(GameEngine::Mesh(GraphicsApi::RenderType::POINTS, graphicsApi, std::move(data), material));
    resourceManager.AddModel(std::move(model));
    return result;
}
Model* CreateLeafModel(IResourceManager& resourceManager, GraphicsApi::IGraphicsApi& graphicsApi,
                       const std::vector<LeafCluster>& leafs, const Material& material)
{
    if (leafs.empty())
    {
        LOG_DEBUG << "Leaf position vector is empty";
        return nullptr;
    }

    LOG_DEBUG << "Create leaf mesh";
    GraphicsApi::MeshRawData data;
    data.positions_.reserve(leafs.size() * 3);
    data.textCoords_.reserve(leafs.size() * 2);
    data.normals_.reserve(leafs.size() * 3);
    data.tangents_.reserve(leafs.size() * 3);

    for (const auto& leaf : leafs)
    {
        data.positions_.push_back(leaf.position.x);
        data.positions_.push_back(leaf.position.y);
        data.positions_.push_back(leaf.position.z);

        data.textCoords_.push_back(leaf.sizeRandomness);
        data.textCoords_.push_back(leaf.radius);

        data.normals_.push_back(leaf.direction.x);
        data.normals_.push_back(leaf.direction.y);
        data.normals_.push_back(leaf.direction.z);

        data.tangents_.push_back(leaf.colorRandomness.x);
        data.tangents_.push_back(leaf.colorRandomness.y);
        data.tangents_.push_back(leaf.colorRandomness.z);

        data.bitangents_.push_back(static_cast<float>(leaf.radius));
        data.bitangents_.push_back(static_cast<float>(leaf.seed));
        data.bitangents_.push_back(0.f);
    }

    auto model  = std::make_unique<GameEngine::Model>();
    auto result = model.get();
    model->AddMesh(GameEngine::Mesh(GraphicsApi::RenderType::POINTS, graphicsApi, std::move(data), material));
    resourceManager.AddModel(std::move(model));
    return result;
}
}  // namespace GameEngine
