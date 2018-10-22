
#include "Scene.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Logger/Log.h"
#include "Utils/Time/Timer.h"

namespace GameEngine
{
Scene::Scene(const std::string& name)
    : objectCount(0)
    , name(name)
    , inputManager_(nullptr)
    , displayManager_(nullptr)
    , renderersManager_(nullptr)
    , physicsApi_(nullptr)
    , gloabalTime(0.f)
    , directionalLight(vec3(10000, 15000, 10000), vec3(0.8))
    , camera(new BaseCamera)
    , componentFactory_(componentController_, time_, resourceManager_, camera, &physicsApi_)
    , simulatePhysics_(true)
{
}

Scene::~Scene()
{
    Log("");

    gameObjects.clear();
    camera.reset();

    if (inputManager_ != nullptr)
    {
        inputManager_->UnsubscribeAll();
    }
}

void Scene::Init()
{
    Initialize();
    componentController_.OnAwake();
    componentController_.OnStart();
}

void Scene::PostInit()
{
}

void Scene::FullUpdate(float deltaTime)
{
    if (physicsApi_ && simulatePhysics_.load())
    {
        Utils::Timer t;
        physicsApi_->SetSimulationStep(deltaTime);
        physicsApi_->Simulate();
        MakeMeasurement("Physics", t.GetTimeMiliseconds());
    }

    if (displayManager_ != nullptr)
    {
        time_.deltaTime = deltaTime;
    }

    Update(deltaTime);
    componentController_.Update();

    if (inputManager_ != nullptr)
    {
        inputManager_->ProcessKeysEvents();
    }
}

void Scene::PostUpdate()
{
    componentController_.PostUpdate();
}

void Scene::AddGameObject(GameObject* object, const vec3& position, const vec3& rotation)
{
    object->RegisterComponentFunctions();
    gameObjects[object->GetId()] = std::unique_ptr<GameObject>(object);
}

void Scene::RemoveGameObject(GameObject* object)
{
    gameObjects.erase(object->GetId());
}

void Scene::SetAddSceneEventCallback(AddEvent func)
{
    addSceneEvent = func;
}

std::list<GameObject*> Scene::GetObjectInRange(const vec3& position, float range)
{
    // int x = static_cast<uint>(position.x / OBJECT_GRID_SIZE);
    // int y = static_cast<uint>(position.z / OBJECT_GRID_SIZE);

    return std::list<GameObject*>();  // m_ObjectInGrid[x + y*OBJECT_GRID_COUNT];
}

ICamera* Scene::GetCamera()
{
    // std::lock_guard<std::mutex> lk(cameraMutex);
    return camera.get();
}

void Scene::SetCamera(std::shared_ptr<ICamera> cam)
{
    // std::lock_guard<std::mutex> lk(cameraMutex);
    camera = std::move(cam);
}

const Light& Scene::GetDirectionalLight() const
{
    return directionalLight;
}

const std::vector<Light>& Scene::GetLights() const
{
    return lights;
}
}  // GameEngine
