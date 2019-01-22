#pragma once
#include <atomic>
#include <list>
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/IComponentFactory.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Particle.h"
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

namespace Input
{
class InputManager;
}

namespace GameEngine
{
class DisplayManager;

namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

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

    inline const std::string& GetName() const;
    inline void RegisterParticleEmitFunction(const std::string& name, EmitFunction f);
    inline std::optional<EmitFunction> GetParticleEmitFunction(const std::string& name) const;

    std::unique_ptr<GameObject> CreateGameObject() const;
    std::unique_ptr<GameObject> CreateGameObject(const std::string& name) const;

    // Add Entities
    void AddGameObject(std::unique_ptr<GameObject>& object);
    void RemoveGameObject(GameObject* object);
    void SetAddSceneEventCallback(AddEvent func);

    // GetObjects
    std::list<GameObject*> GetObjectInRange(const vec3& position, float range);
    inline const GameObjects& GetGameObjects() const;

    // Cameras
    ICamera* GetCamera();
    void SetCamera(std::unique_ptr<ICamera> camera);

    // Lights
    const Light& GetDirectionalLight() const;
    const std::vector<Light>& GetLights() const;
    inline const DayNightCycle& GetDayNightCycle() const;
    inline float GetGlobalTime();

    inline IResourceManager& GetResourceManager();

    void CreateResourceManger(IResourceManager* resourceManager);
    void SetInputManager(Input::InputManager* input);
    void SetRenderersManager(Renderer::RenderersManager* manager);
    void SetDisplayManager(DisplayManager* displayManager);
    void SetPhysicsApi(Physics::IPhysicsApi& physicsApi);
    void SaveToFile(const std::string& filename);
    void LoadFromFile(const std::string& filename);

public:
    uint32 objectCount;

protected:
    virtual int Initialize();
    virtual void PostInitialize();
    virtual int Update(float /*deltaTime*/);

protected:
    std::string name;
    AddEvent addSceneEvent;

    // Renderer::Gui::GuiContext* gui_;
    Input::InputManager* inputManager_;
    DisplayManager* displayManager_;
    Renderer::RenderersManager* renderersManager_;
    Physics::IPhysicsApi* physicsApi_;

    // Minimum one light on scene only (night - decrease strength)
    float gloabalTime = 0.f;
    Light directionalLight;
    std::vector<Light> lights;
    DayNightCycle dayNightCycle;

    std::unique_ptr<ICamera> camera;
    //	std::mutex cameraMutex;

    GameObjects gameObjects;

    Time time_;
    std::shared_ptr<IResourceManager> resourceManager_;
    Components::ComponentController componentController_;
    std::unique_ptr<Components::IComponentFactory> componentFactory_;

    std::atomic_bool simulatePhysics_;
    std::unordered_map<std::string, EmitFunction> emitPatticlesFunctions_;
};

const std::string& Scene::GetName() const
{
    return name;
}

void Scene::RegisterParticleEmitFunction(const std::string& name, EmitFunction f)
{
    emitPatticlesFunctions_.insert({name, f});
}

std::optional<EmitFunction> Scene::GetParticleEmitFunction(const std::string& name) const
{
    return emitPatticlesFunctions_.count(name) ? emitPatticlesFunctions_.at(name) : std::optional<EmitFunction>();
}

inline const GameObjects& Scene::GetGameObjects() const
{
    return gameObjects;
}

inline const DayNightCycle& Scene::GetDayNightCycle() const
{
    return dayNightCycle;
}

// Resources
inline IResourceManager& Scene::GetResourceManager()
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

inline void Scene::SetInputManager(Input::InputManager* input)
{
    inputManager_ = input;
}

inline void Scene::SetRenderersManager(Renderer::RenderersManager* manager)
{
    renderersManager_ = manager;
}

inline void Scene::SetDisplayManager(DisplayManager* displayManager)
{
    displayManager_ = displayManager;
}
inline void Scene::SetPhysicsApi(Physics::IPhysicsApi& physicsApi)
{
    physicsApi_ = &physicsApi;
}
}  // namespace GameEngine
