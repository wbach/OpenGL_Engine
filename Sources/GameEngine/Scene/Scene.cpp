#include "Scene.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Components/ComponentFactory.h"
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
    , componentFactory_(nullptr)
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
    componentFactory_ = std::make_unique<Components::ComponentFactory>(componentController_, time_, *resourceManager_, *renderersManager_, camera, *physicsApi_);
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
        MakeMeasurement("Physics", t.GetTimeNanoseconds());
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

std::unique_ptr<GameObject> Scene::CreateGameObject()
{
    return std::make_unique<GameObject>(*componentFactory_);
}

void Scene::AddGameObject(std::unique_ptr<GameObject>& object)
{
    object->RegisterComponentFunctions();
    gameObjects[object->GetId()] = std::move(object);
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

void Scene::SetCamera(std::unique_ptr<ICamera> cam)
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
