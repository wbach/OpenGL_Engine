#include "NavigationManager.h"

#include <Utils/Variant.h>

#include <memory>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/GridNavigation.h"

namespace GameEngine
{
NavigationManager::NavigationManager()
    : navigationProvider()
{
}
std::vector<vec3> NavigationManager::CalculatePath(const vec3& start, const vec3& end)
{
    if (isDirty)
    {
        ReCreateProvider();
        isDirty = false;
    }

    return navigationProvider ? navigationProvider->CalculatePath(start, end) : std::vector<vec3>{};
}
void NavigationManager::Update(const SceneNotifEvent& event)
{
    std::visit(visitor{[&](const AddGameObjectNotifEvent& e)
                       {
                           if (not e.gameObject)
                               return;

                           if (e.gameObject->HasComponent<Components::TerrainRendererComponent>() or
                               e.gameObject->HasComponent<Components::RendererComponent>())
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
void NavigationManager::ReCreateProvider()
{
    Components::TerrainRendererComponent* terrain = nullptr;
    GameObject* terrainObj                        = nullptr;

    for (auto& [id, obj] : objectInPath)
    {
        if (auto t = obj->GetComponent<Components::TerrainRendererComponent>())
        {
            terrain    = t;
            terrainObj = obj;
            break;  // TO DO: all terrains on scene
        }
    }

    if (not terrain)
    {
        navigationProvider.reset();
        return;
    }

    vec3 terrainScale = terrainObj->GetWorldTransform().GetScale();
    vec3 terrainPos   = terrainObj->GetWorldTransform().GetPosition();

    float cellSize = 1.0f;
    int w          = static_cast<int>(terrainScale.x / cellSize);
    int h          = static_cast<int>(terrainScale.z / cellSize);
    vec3 origin    = terrainPos - (terrainScale * 0.5f);

    navigationProvider = std::make_unique<GridNavigation>(origin, w, h, cellSize);

    TerrainHeightGetter terrainHeightGetter(*terrain);
    navigationProvider->BakeTerrain(terrainHeightGetter, 30.0f);

    int obstacleCount = 0;
    // //TO DO:
    // for (auto& [_, obj] : objectInPath)
    // {
    //     if (auto maybeRendererComponent = obj->GetComponent<Components::RendererComponent>())
    //     {
    //         auto aabb = maybeRendererComponent->getWorldSpaceBoundingBox();
    //         navigationProvider->AddObstacle(aabb);
    //         obstacleCount++;
    //     }
    // }
    LOG_INFO << "Navigation Grid Rebuilt: " << w << "x" << h << " cells, " << obstacleCount << " static obstacles.";
}
}  // namespace GameEngine
