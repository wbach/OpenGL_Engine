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
#include "GameEngine/Renderers/GUI/Text/GuiTextFactory.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Time/DayNightCycle.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "SceneEvents.h"
#include "Types.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"

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

class GuiTextElement;
class GuiTextureElement;
class GuiWindowElement;

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

    void SetDirectionalLightColor(const vec3& color);
    Light& AddLight(const Light& light);

    // Add Entities
    void AddGameObject(std::unique_ptr<GameObject>& object);
    void RemoveGameObject(GameObject* object);
    void SetAddSceneEventCallback(AddEvent func);

    // GetObjects
    inline const GameObjects& GetGameObjects() const;

    // Cameras
    void UpdateCamera();
    const ICamera& GetCamera() const;
    void SetCamera(std::unique_ptr<ICamera> camera);

    // Lights
    const Light& GetDirectionalLight() const;
    const std::vector<Light>& GetLights() const;
    inline const DayNightCycle& GetDayNightCycle() const;
    inline float GetGlobalTime() const;
    inline const Time& GetTime() const;
    inline IResourceManager& GetResourceManager();

    void CreateResourceManger(IResourceManager* resourceManager);
    void SetInputManager(Input::InputManager* input);
    void SetRenderersManager(Renderer::RenderersManager* manager);
    void SetDisplayManager(DisplayManager* displayManager);
    void SetPhysicsApi(Physics::IPhysicsApi& physicsApi);
    void SaveToFile(const std::string& filename);
    void LoadFromFile(const std::string& filename);

public:
    void ReadGuiFile(const std::string& filename);
    GuiTextElement* CreateGuiText(const std::string& label, const std::string& font, const std::string& str, uint32 size, uint32 outline);
    GuiTextureElement* CreateGuiTexture(const std::string& label, const std::string& filename);
    GuiWindowElement* CreateGuiWindow(const std::string& label, const Rect&, const std::string& background);
    GuiTextElement* GuiText(const std::string& label);
    GuiTextureElement* GuiTexture(const std::string& label);

public:
    uint32 objectCount;

protected:
    void MakeGuiManager(std::function<void(GuiElement&)>);
    std::unique_ptr<GuiTextureElement> MakeGuiTexture(const std::string& filename);

    virtual int Initialize();
    virtual void PostInitialize();
    virtual int Update(float /*deltaTime*/);

protected:
    std::string name;
    AddEvent addSceneEvent;

    Input::InputManager* inputManager_;
    DisplayManager* displayManager_;
    Renderer::RenderersManager* renderersManager_;
    Physics::IPhysicsApi* physicsApi_;

    std::unique_ptr<GuiManager> guiManager_;
    std::unique_ptr<GuiElementFactory> guiElementFactory_;

    // Minimum one light on scene only (night - decrease strength)
    float gloabalTime = 0.f;
    Light directionalLight;
    std::vector<Light> lights;
    DayNightCycle dayNightCycle;

    std::unique_ptr<ICamera> camera;

    GameObjects gameObjects;

    Time time_;
    std::unique_ptr<IGuiTextFactory> guiTextFactory_;
    std::shared_ptr<IResourceManager> resourceManager_;
    Components::ComponentController componentController_;
    std::unique_ptr<Components::IComponentFactory> componentFactory_;

    std::atomic_bool simulatePhysics_;
    std::unordered_map<std::string, EmitFunction> emitPatticlesFunctions_;

    std::string lastGuiFileMd5Value_;
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

inline float Scene::GetGlobalTime() const
{
    return gloabalTime;
}

inline const Time& Scene::GetTime() const
{
    return time_;
}

inline void Scene::SetInputManager(Input::InputManager* input)
{
    inputManager_ = input;
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
