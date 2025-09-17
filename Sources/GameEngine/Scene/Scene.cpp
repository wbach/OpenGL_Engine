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
    , dayNightCycle(&directionalLight)
    , simulatePhysics_(true)
    , start_(false)
{
}

Scene::~Scene()
{
    LOG_DEBUG << "destructor";

    if (physicsApi_)
    {
        LOG_DEBUG << "physicsApi_->DisableSimulation()";
        physicsApi_->DisableSimulation();
        LOG_DEBUG << "physicsApi_->DisableSimulation() end";
    }

    if (networkEditorInterface_)
    {
        LOG_DEBUG << "networkEditorInterface_.reset()";
        networkEditorInterface_.reset();
    }

    if (inputManager_)
    {
        LOG_DEBUG << "inputManager_->UnsubscribeAll())";
        inputManager_->UnsubscribeAll();
    }
    if (renderersManager_)
    {
        LOG_DEBUG << "renderersManager_->UnsubscribeAll())";
        renderersManager_->UnSubscribeAll();
    }

    LOG_DEBUG << "destructor done";
}

void Scene::InitResources(EngineContext& context)
{
    engineContext     = &context;
    graphicsApi_      = &context.GetGraphicsApi();
    inputManager_     = &context.GetInputManager();
    physicsApi_       = &context.GetPhysicsApi();
    displayManager_   = &context.GetDisplayManager();
    renderersManager_ = &context.GetRenderersManager();
    threadSync_       = &context.GetThreadSync();
    timerService_     = &context.GetTimerService();
    addEngineEvent    = [&context](EngineEvent event) { context.AddEngineEvent(event); };

    resourceManager_ = context.GetResourceManagerFactory().create();
    if (not resourceManager_)
    {
        LOG_ERROR << "resourceManager  creation error!";
    }
    guiManager_ = std::make_unique<GuiManager>();
    GuiElementFactory::EntryParameters guiFactoryParams{*guiManager_, *inputManager_, *resourceManager_, *renderersManager_};
    guiElementFactory_      = std::make_unique<GuiElementFactory>(guiFactoryParams);
    guiEngineContextManger_ = std::make_unique<GuiEngineContextManger>(context.GetMeasurmentHandler(), *guiElementFactory_);

    console_ = std::make_unique<Debug::Console>(*this);

    componentFactory_ = std::make_unique<Components::ComponentFactory>(
        *this, context.GetSceneManager(), componentController_, context.GetGraphicsApi(), context.GetGpuResourceLoader(), time_,
        *inputManager_, *resourceManager_, *renderersManager_, camera, *physicsApi_, *guiElementFactory_, *timerService_);

    rootGameObject_ = CreateGameObject("root");
}

void Scene::Init()
{
    Initialize();
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
    LOG_DEBUG << "Start";
    start_.store(true);
    componentController_.OnStart();

    if (physicsApi_)
        physicsApi_->EnableSimulation();
    LOG_DEBUG << "Started";
}

void Scene::Stop()
{
    LOG_DEBUG << "Stop";
    start_.store(false);
    if (physicsApi_)
        physicsApi_->DisableSimulation();
}

bool Scene::isStarted() const
{
    return start_.load();
}

void Scene::ChangeName(const std::string& name)
{
    this->name = name;
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::optional<uint32>& maybeId)
{
    return CreateGameObject("gameObject", maybeId);
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::string& name, const std::optional<IdType>& maybeId)
{
    return std::make_unique<GameObject>(name, componentController_, *componentFactory_, gameObjectIdPool_, maybeId);
}

std::unique_ptr<Prefab> Scene::CreatePrefabGameObject(const std::optional<uint32>& maybeId)
{
    return std::make_unique<Prefab>(name, componentController_, *componentFactory_, gameObjectIdPool_, maybeId);
}

std::unique_ptr<Prefab> Scene::CreatePrefabGameObject(const std::string& name, const std::optional<uint32>& maybeId)
{
    return std::make_unique<Prefab>(name, componentController_, *componentFactory_, gameObjectIdPool_, maybeId);
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
    if (not rootGameObject_)
    {
        LOG_WARN << "Root object not exist! Addition aborted! Addition object name:" << object->GetName();
        return;
    }
    gameObjectsIds_.insert({object->GetId(), object.get()});
    auto& ptr = *object;
    rootGameObject_->AddChild(std::move(object));
    rootGameObject_->NotifyComponentControllerAboutObjectCreation(ptr);
}

void Scene::ChangeParent(GameObject& gameObject, GameObject& newParent)
{
    auto currentParent = gameObject.GetParent();

    if (not currentParent)
    {
        LOG_ERROR << "Root gameObject can not be moved";
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

    if (id == 0)
    {
        LOG_WARN << "Deleting of root object!";
        if (rootGameObject_)
        {
            rootGameObject_.reset(nullptr);
            LOG_WARN << "Root object deleted";
            return true;
        }
        LOG_WARN << "RootObject was not set!";
        return false;
    }

    return rootGameObject_->RemoveChild(id);
}

bool Scene::RemoveGameObject(GameObject& object)
{
    gameObjectIdPool_.releaseId(object.GetId());
    gameObjectsIds_.erase(object.GetId());

    if (object.GetId() == 0)
    {
        LOG_WARN << "Deleting of root object!";
        rootGameObject_.reset();
        return true;
    }

    return rootGameObject_->RemoveChild(object);
}

void Scene::ClearGameObjects()
{
    gameObjectIdPool_.clear(1);  // root gameObject stay at id 1
    gameObjectsIds_.clear();

    if (rootGameObject_)
        rootGameObject_->RemoveAllChildren();
}

void Scene::SetAddSceneEventCallback(AddEvent func)
{
    addSceneEvent = func;
}

GameObject* Scene::GetGameObject(uint32 id) const
{
    if (gameObjectsIds_.count(id))
        return gameObjectsIds_.at(id);

    return rootGameObject_->GetChild(id);
}

GameObject* Scene::GetGameObject(const std::string& name) const
{
    if (rootGameObject_)
        return rootGameObject_->GetChild(name);

    return nullptr;
}

GameObject& Scene::GetRootGameObject()
{
    if (not rootGameObject_)
    {
        LOG_DEBUG << "Something went wrong. Not initilized scene?";
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
    }

    networkEditorInterface_->Run();
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

EngineContext* Scene::getEngineContext()
{
    return engineContext;
}

DisplayManager* Scene::getDisplayManager()
{
    return displayManager_;
}

Input::InputManager* Scene::getInputManager()
{
    return inputManager_;
}

Components::ComponentController& Scene::getComponentController()
{
    return componentController_;
}
}  // namespace GameEngine
