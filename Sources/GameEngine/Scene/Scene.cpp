#include "Scene.hpp"

#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/Time/Timer.h>

#include <algorithm>

#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/GUI/GuiEngineContextManger.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "XmlSceneStorage.h"

namespace GameEngine
{
Scene::Scene(const std::string& name)
    : objectCount(0)
    , componentFactory_(nullptr)
    , graphicsApi_(nullptr)
    , inputManager_(nullptr)
    , displayManager_(nullptr)
    , renderersManager_(nullptr)
    , physicsApi_(nullptr)
    , name(name)
    , gloabalTime(0.f)
    , directionalLight(vec3(1000.f, 15000.f, 10000.f), vec3(.8f))
    , simulatePhysics_(true)
    , start_(false)
    , sceneStorage_(std::make_unique<XmlSceneStorage>(*this))
{
}

Scene::~Scene()
{
    if (physicsApi_)
    {
        physicsApi_->DisableSimulation();
    }

    DEBUG_LOG("destructor");

    networkEditorInterface_.reset();

    if (inputManager_ != nullptr)
    {
        inputManager_->UnsubscribeAll();
    }
    if (renderersManager_)
    {
        renderersManager_->UnSubscribeAll();
    }
}

void Scene::InitResources(EngineContext& context)
{
    graphicsApi_      = &context.GetGraphicsApi();
    inputManager_     = &context.GetInputManager();
    physicsApi_       = &context.GetPhysicsApi();
    displayManager_   = &context.GetDisplayManager();
    renderersManager_ = &context.GetRenderersManager();
    threadSync_       = &context.GetThreadSync();
    timerService_     = &context.GetTimerService();
    addEngineEvent    = [&context](EngineEvent event) { context.AddEngineEvent(event); };

    CreateResourceManger(context.GetGraphicsApi(), context.GetGpuResourceLoader());
    guiManager_ = std::make_unique<GuiManager>();
    GuiElementFactory::EntryParameters guiFactoryParams{*guiManager_, *inputManager_, *resourceManager_,
                                                        *renderersManager_};
    guiElementFactory_ = std::make_unique<GuiElementFactory>(guiFactoryParams);
    guiEngineContextManger_ =
        std::make_unique<GuiEngineContextManger>(context.GetMeasurmentHandler(), *guiElementFactory_);

    console_ = std::make_unique<Debug::Console>(*this);

    componentFactory_ = std::make_unique<Components::ComponentFactory>(
        *this, componentController_, context.GetGraphicsApi(), context.GetGpuResourceLoader(), time_, *inputManager_,
        *resourceManager_, *renderersManager_, camera, *physicsApi_, *guiElementFactory_, *timerService_);

    rootGameObject_ = CreateGameObject("root");
}

void Scene::Init()
{
    Initialize();
    Start();
    componentController_.OnStart();
}

void Scene::PostInit()
{
}

void Scene::FullUpdate(float deltaTime)
{
    if (inputManager_)
    {
        inputManager_->ProcessKeysEvents();
    }
    if (guiManager_)
    {
        guiManager_->Update(deltaTime);
    }
    if (guiEngineContextManger_)
    {
        guiEngineContextManger_->Update();
    }
    if (displayManager_)
    {
        time_.deltaTime = deltaTime;
    }
    if (console_)
    {
        console_->ExecuteCommands();
    }

    if (start_.load())
    {
        Update(deltaTime);
        componentController_.Update();
    }

    componentController_.AlwaysUpdate();
}

void Scene::PostUpdate()
{
    if (start_.load())
    {
        componentController_.PostUpdate();
    }
}

void Scene::Start()
{
    DEBUG_LOG("Start");
    sceneStorage_->store();
    start_.store(true);
    if (physicsApi_)
        physicsApi_->EnableSimulation();
}

void Scene::Stop()
{
    DEBUG_LOG("Stop");
    start_.store(false);

    if (physicsApi_)
        physicsApi_->DisableSimulation();

    resourceManager_->LockReleaseResources();
    sceneStorage_->restore();
    resourceManager_->UnlockReleaseResources();
}

bool Scene::isStarted() const
{
    return start_.load();
}

void Scene::CreateResourceManger(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader)
{
    resourceManager_ = std::make_unique<ResourceManager>(graphicsApi, gpuResourceLoader);
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::optional<uint32>& maybeId)
{
    return CreateGameObject("gameObject", maybeId);
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::string& name, const std::optional<IdType>& maybeId)
{
    return std::make_unique<GameObject>(name, componentController_, *componentFactory_, gameObjectIdPool_, maybeId);
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

void Scene::AddGameObject(std::unique_ptr<GameObject> object)
{
    gameObjectsIds_.insert({object->GetId(), object.get()});
    rootGameObject_->AddChild(std::move(object));
}

void Scene::ChangeParent(GameObject& gameObject, GameObject& newParent)
{
    auto currentParent = gameObject.GetParent();

    if (not currentParent)
    {
        DEBUG_LOG("Root gameObject can not be moved");
        return;
    }

    auto worldPosition = gameObject.GetWorldTransform().GetPosition();
    auto worldRotation = gameObject.GetWorldTransform().GetRotation();
    auto worldScale    = gameObject.GetWorldTransform().GetScale();

    auto freeGameObject = currentParent->MoveChild(gameObject.GetId());

    if (freeGameObject)
    {
        auto go = freeGameObject.get();
        newParent.MoveChild(std::move(freeGameObject));
        go->SetWorldPosition(worldPosition);
        go->SetWorldRotation(worldRotation);
        go->SetWorldScale(worldScale);
    }
}

bool Scene::RemoveGameObject(IdType id)
{
    gameObjectIdPool_.releaseId(id);
    gameObjectsIds_.erase(id);
    return rootGameObject_->RemoveChild(id);
}

bool Scene::RemoveGameObject(GameObject& object)
{
    gameObjectIdPool_.releaseId(object.GetId());
    gameObjectsIds_.erase(object.GetId());
    return rootGameObject_->RemoveChild(object);
}

void Scene::ClearGameObjects()
{
    gameObjectIdPool_.clear(1);  // root gameObject stay at id 1
    gameObjectsIds_.clear();
    rootGameObject_->RemoveAllChildren();
}

void Scene::SetAddSceneEventCallback(AddEvent func)
{
    addSceneEvent = func;
}

GameObject* Scene::CloneGameObject(GameObject& gameObject)
{
    return sceneStorage_->clone(gameObject);
}

GameObject* Scene::GetGameObject(uint32 id) const
{
    if (gameObjectsIds_.count(id))
        return gameObjectsIds_.at(id);

    return rootGameObject_->GetChild(id);
}

GameObject* Scene::GetGameObject(const std::string& name) const
{
    return rootGameObject_->GetChild(name);
}

GameObject &Scene::GetRootGameObject()
{
    if (not rootGameObject_)
    {
        ERROR_LOG("Something went wrong. Not initilized scene?");
    }
    return *rootGameObject_;
}

void Scene::UpdateCamera()
{
    camera.Update();
}

CameraWrapper& Scene::GetCamera()
{
    return camera;
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
void Scene::SaveToFile()
{
    DEBUG_LOG("Save scene to file : " + file_.GetAbsoultePath());
    sceneStorage_->saveToFile(file_);
    DEBUG_LOG("Scene save complete  , file : " + file_.GetAbsoultePath());
}
void Scene::SaveToFile(const File& file)
{
    DEBUG_LOG("Save scene to file : " + file.GetAbsoultePath());
    sceneStorage_->saveToFile(file);
    DEBUG_LOG("Scene save complete  , file : " + file.GetAbsoultePath());
}
void Scene::LoadFromFile(const File& file)
{
    DEBUG_LOG("Load scene from file : " + file.GetAbsoultePath());
    file_ = file;
    sceneStorage_->readFromFile(file);
    DEBUG_LOG("Load scene from file : \"" + file.GetAbsoultePath() + "\" complete");
}

GameObject* Scene::LoadPrefab(const File& file, const std::string& gameObjectName)
{
    return sceneStorage_->loadPrefab(file, gameObjectName);
}

void Scene::CreatePrefab(const File& file, const GameObject& gameObject)
{
    sceneStorage_->createPrefab(file, gameObject);
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
    if (not networkEditorInterface_)
    {
        networkEditorInterface_ = std::make_unique<NetworkEditorInterface>(*this, *threadSync_);
        networkEditorInterface_->Run();
    }
    else
    {
        networkEditorInterface_->Run();
    }
}
void Scene::StopNetworkEditorInterface()
{
    networkEditorInterface_.reset();
}
std::optional<Physics::RayHit> Scene::getHeightPositionInWorld(float x, float z) const
{
    return physicsApi_->RayTest(vec3(x, 10000, z), vec3(x, -10000, z));
}
float Scene::distanceToCamera(const GameObject& gameObject) const
{
    return glm::length(camera.GetPosition() - gameObject.GetWorldTransform().GetPosition());
}

void Scene::SendEvent(SceneEvent& event)
{
    addSceneEvent(event);
}

void Scene::SendEvent(EngineEvent& event)
{
    addEngineEvent(event);
}

DisplayManager* Scene::getDisplayManager()
{
    return displayManager_;
}

Input::InputManager* Scene::getInputManager()
{
    return inputManager_;
}
}  // namespace GameEngine
