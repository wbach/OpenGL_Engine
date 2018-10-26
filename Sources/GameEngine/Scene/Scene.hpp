#pragma once
#include <atomic>
#include <list>
#include <memory>
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Renderers/GUI/GuiContext.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Time/DayNightCycle.h"
#include "SceneEvents.h"
#include "Types.h"
//#include "Mutex.hpp"

// Object in scene are in grid (one grid size)
const uint32 OBJECT_GRID_SIZE = 500;
// const int grid count y in worlds
const uint32 OBJECT_GRID_COUNT = 100;

namespace GameEngine
{
class DisplayManager;
class InputManager;

namespace Renderer
{
class RenderersManager;
}  // Renderer

typedef std::unordered_map<uint32, std::unique_ptr<GameObject>> GameObjects;

class Scene
{
public:
    Scene(const std::string& name);

    virtual ~Scene();

    void Init();
    void PostInit();
    void FullUpdate(float deltaTime);
    void PostUpdate();

    const std::string& GetName()
    {
        return name;
    }

    // Add Entities
    void AddGameObject(GameObject* object, const vec3& position = vec3(0.f), const vec3& rotation = vec3(0.0f));
    void RemoveGameObject(GameObject* object);
    void SetAddSceneEventCallback(AddEvent func);

    // GetObjects
    std::list<GameObject*> GetObjectInRange(const vec3& position, float range);
    inline const GameObjects& GetGameObjects();

    // Cameras
    ICamera* GetCamera();
    void SetCamera(std::shared_ptr<ICamera> camera);

    // Lights
    const Light& GetDirectionalLight() const;
    const std::vector<Light>& GetLights() const;
    inline const DayNightCycle& GetDayNightCycle() const;
    inline float GetGlobalTime();

    inline IResourceManager& GetResourceManager();

    void CreateResourceManger(IResourceManager* resourceManager);
    void SetInputManager(InputManager* input);
    void SetRenderersManager(Renderer::RenderersManager* manager);
    void SetDisplayManager(DisplayManager* displayManager);
    void SetPhysicsApi(Physics::IPhysicsApiPtr physicsApi);

    template <class T>
    T* AddComponent(GameObject* obj)
    {
        auto comp = componentFactory_.Create(T::type, obj);
        auto r    = comp.get();
        obj->AddComponent(std::move(comp));
        return static_cast<T*>(r);
    }

public:
    uint32 objectCount;

protected:
    virtual int Initialize()
    {
        return 0;
    }
    virtual void PostInitialize()
    {
    }
    virtual int Update(float /*deltaTime*/)
    {
        return 0;
    }

protected:
    std::string name;
    AddEvent addSceneEvent;

    // Renderer::Gui::GuiContext* gui_;
    InputManager* inputManager_;
    DisplayManager* displayManager_;
    Renderer::RenderersManager* renderersManager_;
    Physics::IPhysicsApiPtr physicsApi_;

    // Minimum one light on scene only (night - decrease strength)
    float gloabalTime = 0.f;
    Light directionalLight;
    std::vector<Light> lights;
    DayNightCycle dayNightCycle;

    std::shared_ptr<ICamera> camera;
    //	std::mutex cameraMutex;

    GameObjects gameObjects;

    Time time_;
    std::shared_ptr<IResourceManager> resourceManager_;
    Components::ComponentController componentController_;
    Components::ComponentFactory componentFactory_;

    std::atomic_bool simulatePhysics_;
};

inline const GameObjects& Scene::GetGameObjects()
{
    return gameObjects;
}

inline const DayNightCycle& Scene::GetDayNightCycle() const
{
    return dayNightCycle;
}

// Resources
inline IResourceManager &Scene::GetResourceManager()
{
    return *resourceManager_;
}

inline float Scene::GetGlobalTime()
{
    return gloabalTime;
}

inline void Scene::CreateResourceManger(IResourceManager* resourceManager)
{
    resourceManager_.reset(resourceManager);
}

inline void Scene::SetInputManager(InputManager* input)
{
    inputManager_ = input;
}

inline void Scene::SetRenderersManager(Renderer::RenderersManager* manager)
{
    renderersManager_ = manager;
    componentFactory_.SetRendererManager(manager);
}

inline void Scene::SetDisplayManager(DisplayManager* displayManager)
{
    displayManager_ = displayManager;
}
inline void Scene::SetPhysicsApi(Physics::IPhysicsApiPtr physicsApi)
{
    physicsApi_ = physicsApi;
}
}  // GameEngine
