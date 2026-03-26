#pragma once
#include <memory>
#include <set>
#include <unordered_map>

#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/SceneEvents.h"
#include "INavigationProvider.h"
#include "Types.h"

namespace GameEngine
{
class Scene;
class ENGINE_API NavigationManager
{
public:
    NavigationManager(Physics::IPhysicsApi&);

    void Update();
    void Update(const SceneNotifEvent&);
    std::vector<vec3> CalculatePath(const vec3&, const vec3&);
    std::shared_ptr<INavigationProvider> GetNavigationProvider() const;
    void lock();
    void unlock();
    void ReCreateProvider();

private:
    Physics::IPhysicsApi& physicsApi;
    std::shared_ptr<INavigationProvider> navigationProvider;
    std::unordered_map<IdType, GameObject*> objectInPath;
    bool isDirty{false};
    mutable std::mutex providerMutex;
};
}  // namespace GameEngine
