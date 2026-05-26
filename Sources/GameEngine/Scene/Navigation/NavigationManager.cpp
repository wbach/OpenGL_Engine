#include "NavigationManager.h"

#include <Utils/Variant.h>

#include <limits>
#include <memory>
#include <mutex>

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Layer.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Scene/Navigation/GridNavigation.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
const float defaultAgentRadius = 0.4f;
}
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

                           auto hasTerrainComponent = e.gameObject->HasComponent<Components::TerrainRendererComponent>();
                           auto hasRenderComponent  = e.gameObject->HasComponent<Components::RendererComponent>();
                           auto rigidbody           = e.gameObject->GetComponent<Components::Rigidbody>();

                           if (hasTerrainComponent)
                           {
                               staticObjects.insert({e.gameObject->GetId(), e.gameObject});
                               isDirty = true;
                           }
                           else if (rigidbody)
                           {
                               if (not rigidbody->IsStatic())
                               {
                                   LOG_DEBUG << "Dynamic : " << e.gameObject->GetName();
                                   dynamicObjects[e.gameObject->GetId()] = DynamicObjectInfo{
                                       .gameObject   = e.gameObject,
                                       .lastPosition = e.gameObject->GetWorldTransform().GetPosition(),
                                   };
                               }
                               else
                               {
                                   LOG_DEBUG << "Static : " << e.gameObject->GetName();
                                   staticObjects.insert({e.gameObject->GetId(), e.gameObject});
                                   isDirty = true;
                               }
                           }
                           else if (hasRenderComponent)
                           {
                               LOG_DEBUG << "Static : " << e.gameObject->GetName();
                               staticObjects.insert({e.gameObject->GetId(), e.gameObject});
                               isDirty = true;
                           }
                       },
                       [&](const ModifyGameObjectEvent& e) {}, [&](const ChangeParentEvent&) {},
                       [&](const ClearGameObjectsEvent&)
                       {
                           staticObjects.clear();
                           dynamicObjects.clear();
                           navigationProvider.reset();
                           isDirty = false;
                       },
                       [&](const RemoveGameObjectEvent& e)
                       {
                           if (staticObjects.erase(e.gameObjectId))
                           {
                               isDirty = true;
                           }

                           auto iter = dynamicObjects.find(e.gameObjectId);
                           if (iter != dynamicObjects.end())
                           {
                               navigationProvider->RemovePhysicsObstacle(iter->second.grids);
                               dynamicObjects.erase(iter);
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

    UpdateDynamicObjects();
}
void NavigationManager::ReCreateProvider()
{
    LOG_DEBUG << "";
    std::lock_guard lk(providerMutex);
    Components::TerrainRendererComponent* terrain = nullptr;
    GameObject* terrainObj                        = nullptr;

    for (auto& [id, info] : dynamicObjects)
    {
        info.grids.clear();
        info.lastPosition = vec3(std::numeric_limits<float>::max());
    }

    for (auto& [id, obj] : staticObjects)
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

    int staticObstacleCount = 0;
    for (auto& [_, obj] : staticObjects)
    {
        if (obj == terrainObj)
            continue;

        if (auto maybRigidbody = obj->GetComponent<Components::Rigidbody>())
        {
            if (auto pBB = physicsApi.getBoundingBox(maybRigidbody->GetId()))
            {
                LOG_DEBUG << "AddPhysicsObstacle static : " << obj->GetName() << " getId " << obj->GetId();
                navigationProvider->AddPhysicsObstacle(physicsApi, *maybRigidbody->GetId(), *pBB, defaultAgentRadius);
                staticObstacleCount++;
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

    LOG_INFO << "Navigation Grid Rebuilt: " << w << "x" << h << " cells, " << staticObstacleCount << " static obstacles.";
}
std::shared_ptr<INavigationProvider> NavigationManager::GetNavigationProvider() const
{
    std::lock_guard<std::mutex> lock(providerMutex);
    return navigationProvider;
}
bool NavigationManager::Raycast(const vec3& start, const vec3& end)
{
    std::lock_guard lk(providerMutex);
    if (not navigationProvider)
    {
        return false;
    }

    return navigationProvider->Raycast(start, end);
}
void NavigationManager::UpdateDynamicObjects()
{
    if (not navigationProvider)
    {
        return;
    }

    for (auto& [id, info] : dynamicObjects)
    {
        auto* obj = info.gameObject;

        if (not obj)
            continue;

        auto currentPos = obj->GetWorldTransform().GetPosition();

        auto notEqual = glm::epsilonNotEqual(currentPos, info.lastPosition, 0.001f);
        if (glm::any(notEqual))
        {
            auto rigidbody = obj->GetComponent<Components::Rigidbody>();
            if (not rigidbody)
                continue;

            std::lock_guard lk(providerMutex);

            if (auto pBB = physicsApi.getBoundingBox(rigidbody->GetId()))
            {
                navigationProvider->RemovePhysicsObstacle(info.grids);

                info.grids =
                    navigationProvider->AddPhysicsObstacle(physicsApi, *rigidbody->GetId(), *pBB, defaultAgentRadius);
                info.lastPosition = currentPos;
            }
            else
            {
                LOG_DEBUG << "Dynamic BB not ready for partial update : " << obj->GetName();
            }
        }
    }
}
}  // namespace GameEngine
