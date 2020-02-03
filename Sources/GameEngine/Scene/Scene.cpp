#include "Scene.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"
#include "SceneReader.h"
#include "SceneWriter.h"
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
    , componentFactory_(nullptr)
    , simulatePhysics_(true)
{
}

Scene::~Scene()
{
    gameObjects.clear();
    if (inputManager_ != nullptr)
    {
        inputManager_->UnsubscribeAll();
    }
    if (renderersManager_)
    {
        renderersManager_->UnSubscribeAll();
    }
}

void Scene::InitResources(SceneInitContext& context)
{
    inputManager_     = context.inputManager;
    physicsApi_       = context.physicsApi;
    displayManager_   = context.displayManager;
    renderersManager_ = context.renderersManager;

    CreateResourceManger(*context.graphicsApi_);
    guiManager_ = std::make_unique<GuiManager>();
    GuiElementFactory::EntryParameters guiFactoryParams{*guiManager_, *inputManager_, *resourceManager_, *renderersManager_};
    guiElementFactory_ = std::make_unique<GuiElementFactory>(guiFactoryParams);

    console_ = std::make_unique<Debug::Console>(*this);
}

void Scene::Init()
{
    componentFactory_ = std::make_unique<Components::ComponentFactory>(componentController_, time_, *inputManager_, *resourceManager_, *renderersManager_, camera, *physicsApi_);
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
        physicsApi_->SetSimulationStep(deltaTime);
        physicsApi_->Simulate();
    }
    if (displayManager_)
    {
        time_.deltaTime = deltaTime;
    }
    if (inputManager_)
    {
        inputManager_->ProcessKeysEvents();
    }
    if (guiManager_)
    {
        guiManager_->Update();
    }

    Update(deltaTime);
    componentController_.Update();
}

void Scene::PostUpdate()
{
    componentController_.PostUpdate();
}

void Scene::CreateResourceManger(GraphicsApi::IGraphicsApi& graphicsApi)
{
    resourceManager_ = std::make_unique<ResourceManager>(graphicsApi);
}

std::unique_ptr<GameObject> Scene::CreateGameObject() const
{
    return std::make_unique<GameObject>("gameObject", *componentFactory_);
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::string& name) const
{
    return std::make_unique<GameObject>(name, *componentFactory_);
}

void Scene::SetDirectionalLightColor(const vec3& color)
{
    directionalLight.SetColor(color);
}

Light& Scene::AddLight(const Light& light)
{
    lights.push_back(light);
    return lights.back();
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

void Scene::SetAddEngineEventCallback(std::function<void(EngineEvent)> func)
{
    addEngineEvent = func;
}

void Scene::UpdateCamera()
{
    camera.Update();
}

void Scene::SetCamera(ICamera& cam)
{
    camera.Set(cam);
}

const CameraWrapper& Scene::GetCamera() const
{
    return camera;
}

const Light& Scene::GetDirectionalLight() const
{
    return directionalLight;
}

const std::vector<Light>& Scene::GetLights() const
{
    return lights;
}
void Scene::SaveToFile(const std::string& filename)
{
    SaveSceneState(*this, filename);
}
void Scene::LoadFromFile(const std::string& filename)
{
    DEBUG_LOG("Load scene from file : " + filename);
    SceneReader::LoadScene(*this, filename);
}

void Scene::LoadPrefab(const std::string& filename, const std::string& name)
{
    SceneReader::LoadPrefab(*this, filename, name);
}

int Scene::Initialize()
{
    return 0;
}
void Scene::PostInitialize()
{
}
int Scene::Update(float)
{
    return 0;
}

void Scene::RunNetworkEditorInterface()
{
    networkEditorInterface_ = std::make_unique<NetworkEditorInterface>(*this);
}
void Scene::StopNetworkEditorInterface()
{
    networkEditorInterface_.reset();
}
}  // namespace GameEngine
