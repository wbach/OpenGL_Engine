#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Utils/IdPool.h>
#include <Utils/Time/TimerService.h>

#include <atomic>
#include <memory>
#include <optional>
#include <vector>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/DebugTools/Console/Console.h"
#include "GameEngine/DebugTools/EditorInterface/NetworkEditorInterface.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Particle.h"
#include "GameEngine/Objects/Prefab.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Time/DayNightCycle.h"
#include "SceneEvents.h"
#include "Types.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;

namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

class GuiTextElement;
class GuiTextureElement;
class GuiWindowElement;
class GuiEngineContextManger;

class Scene
{
public:
    using Events = std::vector<SceneEvent>;

    Scene(const std::string& name);
    virtual ~Scene();

    void InitResources(EngineContext&);
    void Init();
    void PostInit();
    void FullUpdate(float deltaTime);
    void PostUpdate();
    void Start();
    void Stop();
    bool isStarted() const;

    void ChangeName(const std::string& name);
    inline const std::string& GetName() const;
    inline File& GetFile();
    inline const File& GetFile() const;
    inline void RegisterParticleEmitFunction(const std::string& name, EmitFunction f);
    inline std::optional<EmitFunction> GetParticleEmitFunction(const std::string& name) const;

    std::unique_ptr<GameObject> CreateGameObject(const std::optional<uint32>& = std::nullopt);
    std::unique_ptr<GameObject> CreateGameObject(const std::string&, const std::optional<uint32>& = std::nullopt);
    std::unique_ptr<Prefab> CreatePrefabGameObject(const std::optional<uint32>& = std::nullopt);
    std::unique_ptr<Prefab> CreatePrefabGameObject(const std::string&, const std::optional<uint32>& = std::nullopt);

    void SetDirectionalLightColor(const vec3& color);
    Light& AddLight(const Light& light);

    // Add Entities
    void AddGameObject(std::unique_ptr<GameObject>);
    void AddGameObject(GameObject&, std::unique_ptr<GameObject>);

    void ChangeParent(IdType, IdType);
    void ChangeParent(GameObject&, GameObject&);
    void RemoveParent(GameObject&);
    void RemoveGameObject(IdType);
    void RemoveGameObject(GameObject&);
    void ClearGameObjects();

    // GetObjects
    inline const GameObjects& GetGameObjects() const;
    const std::unordered_map<uint32, GameObject*> GetAllGameObjectsPtrs() const;
    GameObject* GetGameObject(uint32) const;
    GameObject* GetGameObject(const std::string&) const;
    GameObject& GetRootGameObject();

    // Cameras
    void UpdateCamera();
    CameraWrapper& GetCamera();
    const CameraWrapper& GetCamera() const;

    // Lights
    const Light& GetDirectionalLight() const;
    const std::vector<Light>& GetLights() const;
    inline const DayNightCycle& GetDayNightCycle() const;
    inline DayNightCycle& GetDayNightCycle();
    inline float GetGlobalTime() const;
    inline const Time& GetTime() const;
    inline IResourceManager& GetResourceManager();

    void RunNetworkEditorInterface();
    void StopNetworkEditorInterface();

    std::optional<Physics::RayHit> getHeightPositionInWorld(float, float) const;
    float distanceToCamera(const GameObject&) const;

    void SendEvent(SceneEvent&&);
    void SendEvent(EngineEvent&);

    EngineContext* getEngineContext();
    DisplayManager* getDisplayManager();
    Input::InputManager* getInputManager();
    Components::ComponentController& getComponentController();

public:
    uint32 objectCount;
    std::function<void(EngineEvent)> addEngineEvent;

protected:
    virtual int Initialize();
    virtual void PostInitialize();
    virtual int Update(float /*deltaTime*/);

protected:
    std::unique_ptr<IResourceManager> resourceManager_;
    CameraWrapper camera;

    Components::ComponentController componentController_;
    std::unique_ptr<Components::ComponentFactory> componentFactory_;
    std::unique_ptr<GuiManager> guiManager_;
    std::unique_ptr<GuiElementFactory> guiElementFactory_;
    std::unique_ptr<GuiEngineContextManger> guiEngineContextManger_;

    std::unique_ptr<GameObject> rootGameObject_;
    std::unordered_map<uint32, GameObject*> gameObjectsIds_;

    EngineContext* engineContext{nullptr};
    GraphicsApi::IGraphicsApi* graphicsApi_{nullptr};
    Input::InputManager* inputManager_{nullptr};
    DisplayManager* displayManager_{nullptr};
    Renderer::RenderersManager* renderersManager_{nullptr};
    Physics::IPhysicsApi* physicsApi_{nullptr};
    Utils::Thread::ThreadSync* threadSync_{nullptr};
    Utils::Time::TimerService* timerService_{nullptr};

    std::string name;
    File file_;

    // Minimum one light on scene only (night - decrease strength)
    Time time_;
    float gloabalTime = 0.f;
    Light directionalLight;
    std::vector<Light> lights;
    DayNightCycle dayNightCycle;

    std::atomic_bool simulatePhysics_;
    std::unordered_map<std::string, EmitFunction> emitPatticlesFunctions_;
    Utils::IdPool gameObjectIdPool_;

private:
    void ProcessEvents();
    void ProcessEvent(AddGameObjectEvent&&);
    void ProcessEvent(ModifyGameObjectEvent&&);
    void ProcessEvent(RemoveGameObjectEvent&&);
    void ProcessEvent(ClearGameObjectsEvent&&);
    void ProcessEvent(ChangeParentEvent&&);

private:
    std::mutex eventsMutex;
    Events events;

    std::atomic_bool start_;
    std::unique_ptr<NetworkEditorInterface> networkEditorInterface_;
    std::unique_ptr<Debug::Console> console_;

    friend class NetworkEditorInterface;
    friend class Debug::Console;
};

const std::string& Scene::GetName() const
{
    return name;
}

File& Scene::GetFile()
{
    return file_;
}

inline const File& Scene::GetFile() const
{
    return file_;
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
    return rootGameObject_->GetChildren();
}

inline const std::unordered_map<uint32, GameObject*> Scene::GetAllGameObjectsPtrs() const
{
    return gameObjectsIds_;
}

inline const DayNightCycle& Scene::GetDayNightCycle() const
{
    return dayNightCycle;
}

inline DayNightCycle& Scene::GetDayNightCycle()
{
    return dayNightCycle;
}

// Resources
inline IResourceManager& Scene::GetResourceManager()
{
    return *resourceManager_;
}

inline float Scene::GetGlobalTime() const
{
    return gloabalTime;
}

inline const Time& Scene::GetTime() const
{
    return time_;
}
}  // namespace GameEngine
