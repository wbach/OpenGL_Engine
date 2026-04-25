#include "NavigationManager.h"

#include <Utils/Variant.h>

#include <memory>
#include <mutex>

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Layer.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Scene/Navigation/GridNavigation.h"
#include "Logger/Log.h"

namespace GameEngine
{
NavigationManager::NavigationManager(Physics::IPhysicsApi& physicsApi)
    : physicsApi(physicsApi)
{
}
std::vector<vec3> NavigationManager::CalculatePath(const vec3& start, const vec3& end)
{
    std::lock_guard lk(providerMutex);
    return navigationProvider ? navigationProvider->CalculatePath(start, end) : std::vector<vec3>{};
}
void NavigationManager::Update(const SceneNotifEvent& event)
{
    std::visit(visitor{[&](const AddGameObjectNotifEvent& e)
                       {
                           if (not e.gameObject)
                               return;

                           if (not e.gameObject->IsLayer(Layer::NoNavMesh))
                               return;

                           if (e.gameObject->HasComponent<Components::TerrainRendererComponent>() or
                               e.gameObject->HasComponent<Components::RendererComponent>() or
                               e.gameObject->HasComponent<Components::Rigidbody>())
                           {
                               objectInPath.insert({e.gameObject->GetId(), e.gameObject});
                               isDirty = true;
                           }
                       },
                       [&](const ModifyGameObjectEvent& e) {}, [&](const ChangeParentEvent&) {},
                       [&](const ClearGameObjectsEvent&)
                       {
                           objectInPath.clear();
                           navigationProvider.reset();
                           isDirty = false;
                       },
                       [&](const RemoveGameObjectEvent& e)
                       {
                           if (objectInPath.erase(e.gameObjectId))
                           {
                               isDirty = true;
                           }
                       }},
               event);
}
void NavigationManager::Update()
{
    if (isDirty)
    {
        ReCreateProvider();
        isDirty = false;
    }
}
void NavigationManager::ReCreateProvider()
{
    std::lock_guard lk(providerMutex);
    Components::TerrainRendererComponent* terrain = nullptr;
    GameObject* terrainObj                        = nullptr;

    for (auto& [id, obj] : objectInPath)
    {
        if (auto t = obj->GetComponent<Components::TerrainRendererComponent>())
        {
            if (t->GetHeightMap())  // check is initalized
            {
                terrain    = t;
                terrainObj = obj;
                break;  // TO DO: all terrains on scene
            }
        }
    }

    if (not terrain)
    {
        navigationProvider.reset();
        return;
    }

    vec3 terrainScale = terrainObj->GetWorldTransform().GetScale();
    vec3 terrainPos   = terrainObj->GetWorldTransform().GetPosition();

    float cellSize = 0.5f;  // 1.f;  // 0.3f;
    int w          = static_cast<int>(terrainScale.x / cellSize);
    int h          = static_cast<int>(terrainScale.z / cellSize);
    vec3 origin    = terrainPos - (terrainScale * 0.5f);

    navigationProvider = std::make_unique<GridNavigation>(origin, w, h, cellSize);

    TerrainHeightGetter terrainHeightGetter(*terrain);
    navigationProvider->BakeTerrain(terrainHeightGetter, 30.0f);

    const float defaultAgentRadius = 0.4f;

    int obstacleCount = 0;
    for (auto& [_, obj] : objectInPath)
    {
        if (obj == terrainObj)
            continue;

        if (auto maybRigidbody = obj->GetComponent<Components::Rigidbody>())
        {
            if (auto pBB = physicsApi.getBoundingBox(maybRigidbody->GetId()))
            {
                navigationProvider->AddPhysicsObstacle(physicsApi, *pBB, defaultAgentRadius);
                obstacleCount++;
            }
            else
            {
                LOG_DEBUG << "BB not ready";
            }
        }
        // else if (auto maybeRendererComponent = obj->GetComponent<Components::RendererComponent>())
        // {
        //     auto model = maybeRendererComponent->GetModelWrapper().Get();
        //     if (not model)
        //     {
        //         LOG_DEBUG << "Model not ready";
        //         continue;
        //     }

        //     const auto& worldMatrix = obj->GetWorldTransform().CalculateCurrentMatrix();
        //     navigationProvider->AddObstacle(*model, worldMatrix, defaultAgentRadius);

        //     obstacleCount++;
        // }
    }
    LOG_INFO << "Navigation Grid Rebuilt: " << w << "x" << h << " cells, " << obstacleCount << " static obstacles.";
}
std::shared_ptr<INavigationProvider> NavigationManager::GetNavigationProvider() const
{
    std::lock_guard<std::mutex> lock(providerMutex);
    return navigationProvider;
}
}  // namespace GameEngine
