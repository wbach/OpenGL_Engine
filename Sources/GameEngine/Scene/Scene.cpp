#include "Scene.hpp"

#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Types.h>
#include <Utils/Time/Timer.h>
#include <Variant.h>

#include <algorithm>
#include <mutex>
#include <utility>

#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/GUI/GuiEngineContextManger.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IResourceManagerFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Scene/SceneEvents.h"
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
    , windParams(GetWindPreset(2))
    , simulatePhysics_(true)
    , start_(false)
{
}

Scene::~Scene()
{
    LOG_DEBUG << "destructor";
    Stop();

    if (rootGameObject_)
    {
        LOG_DEBUG << "Removing all game objects";
        rootGameObject_->RemoveAllChildren();
        LOG_DEBUG << "All game objects removed";
    }
    componentController_.UnRegisterAll();

    if (guiManager_)
    {
        guiManager_.reset();
    }

    if (guiElementFactory_)
    {
        guiElementFactory_.reset();
    }

    if (guiEngineContextManger_)
    {
        guiEngineContextManger_.reset();
    }

    if (console_)
    {
        console_.reset();
    }

    if (componentFactory_)
    {
        componentFactory_.reset();
    }

    if (networkEditorInterface_)
    {
        LOG_DEBUG << "networkEditorInterface_.reset()";
        networkEditorInterface_.reset();
    }

    if (inputManager_)
    {
        LOG_DEBUG << "inputManager_->UnsubscribeAll()";
        inputManager_->UnsubscribeAll();
    }
    if (renderersManager_)
    {
        LOG_DEBUG << "renderersManager_->UnsubscribeAll()";
        renderersManager_->UnSubscribeAll();
    }

    rootGameObject_.reset();

    LOG_DEBUG << "destructor done";
}

void Scene::InitResources(EngineContext& context)
{
    LOG_DEBUG << "";
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

    ProcessEvents();
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

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::optional<IdType>& maybeId)
{
    return CreateGameObject("gameObject", maybeId);
}

std::unique_ptr<GameObject> Scene::CreateGameObject(const std::string& name, const std::optional<IdType>& maybeId)
{
    return std::make_unique<GameObject>(name, componentController_, *componentFactory_, gameObjectIdPool_, maybeId);
}

std::unique_ptr<Prefab> Scene::CreatePrefabGameObject(const std::optional<IdType>& maybeId)
{
    return std::make_unique<Prefab>(name, componentController_, *componentFactory_, gameObjectIdPool_, maybeId);
}

std::unique_ptr<Prefab> Scene::CreatePrefabGameObject(const std::string& name, const std::optional<IdType>& maybeId)
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

void Scene::ProcessEvents()
{
    Events tmpEvents;
    {
        std::lock_guard<std::mutex> lk(eventsMutex);
        tmpEvents = std::move(events);
    }

    for (auto&& event : tmpEvents)
    {
        std::visit(visitor{[&](auto&& e) { ProcessEvent(std::forward<decltype(e)>(e)); }}, std::move(event));
    }
}

void Scene::ProcessEvent(AddGameObjectEvent&& event)
{
    auto parentGameObject = GetGameObject(event.parentGameObject);

    if (not parentGameObject)
    {
        LOG_WARN << "Parent object not exist! Addition aborted! Addition object name:" << event.gameObject->GetName();
        return;
    }
    auto ptr = event.gameObject.get();
    gameObjectsIds_.insert({event.gameObject->GetId(), ptr});
    parentGameObject->AddChild(std::move(event.gameObject));

    auto notifyComponentController = [&](auto&& self, GameObject& gameObject) -> void
    {
        for (auto& subChild : gameObject.children_)
        {
            if (subChild)
            {
                self(self, *subChild);
            }
        }
        componentController_.OnObjectCreated(gameObject.GetId());
    };
    notifyComponentController(notifyComponentController, *ptr);

    LOG_DEBUG << "Game object added. Addition object name:" << ptr->GetName();

    auto notifySceneEventSubscribers = [&](auto&& self, uint32 parentId, GameObject* gameObject) -> void
    {
        NotifySceneEventSubscribers(AddGameObjectNotifEvent{.parentGameObject = parentId, .gameObject = gameObject});

        LOG_DEBUG << gameObject->GetName() << " childs " << gameObject->children_.size();

        for (auto& subChild : gameObject->children_)
        {
            if (subChild)
            {
                self(self, gameObject->GetId(), subChild.get());
            }
        }
    };
    notifySceneEventSubscribers(notifySceneEventSubscribers, parentGameObject->GetId(), ptr);
}

void Scene::ProcessEvent(ModifyGameObjectEvent&& event)
{
    auto gameObject = GetGameObject(event.gameObjectId);
    if (not gameObject)
    {
        LOG_WARN << "ModifyGameObjectEvent error. GameObject not found: " << event.gameObjectId;
        return;
    }

    if (event.worldTransform)
    {
        if (event.worldTransform->position)
        {
            gameObject->SetWorldPosition(event.worldTransform->position.value());
        }
        if (event.worldTransform->rotation)
        {
            gameObject->SetWorldRotation(event.worldTransform->rotation.value());
        }
        if (event.worldTransform->scale)
        {
            gameObject->SetWorldScale(event.worldTransform->scale.value());
        }
    }

    if (event.localTransform)
    {
        if (event.localTransform->position)
        {
            gameObject->localTransform_.SetPosition(event.localTransform->position.value());
        }
        if (event.worldTransform->rotation)
        {
            gameObject->localTransform_.SetRotation(event.localTransform->rotation.value());
        }
        if (event.localTransform->scale)
        {
            gameObject->localTransform_.SetScale(event.localTransform->scale.value());
        }
    }

    LOG_DEBUG << "Game object modified. Object name:" << gameObject->GetName();

    NotifySceneEventSubscribers(event);
}

void Scene::ProcessEvent(RemoveGameObjectEvent&& event)
{
    LOG_DEBUG << "RemoveGameObjectEvent id=" << event.gameObjectId;
    auto id = event.gameObjectId;

    if (id == 0)
    {
        LOG_WARN << "Deleting of root object!";
        if (rootGameObject_)
        {
            rootGameObject_.reset(nullptr);
            LOG_WARN << "Root object deleted";
            return;
        }
        LOG_WARN << "RootObject was not set!";
    }

    auto ids = rootGameObject_->RemoveChild(id);
    for (const auto id : ids)
    {
        LOG_DEBUG << "RemoveGameObjectEvent id=" << event.gameObjectId << ", childId= " << id;
        gameObjectsIds_.erase(id);
    }

    NotifySceneEventSubscribers(event);
}
void Scene::ProcessEvent(ClearGameObjectsEvent&& event)
{
    gameObjectIdPool_.clear(1);  // root gameObject stay at id 1
    gameObjectsIds_.clear();

    if (rootGameObject_)
        rootGameObject_->RemoveAllChildren();

    NotifySceneEventSubscribers(event);
}

void Scene::ProcessEvent(ChangeParentEvent&& event)
{
    auto currentGameObject = GetGameObject(event.gameObjectId);
    if (not currentGameObject)
    {
        LOG_WARN << "Change parent error. GameObject not found: " << event.gameObjectId;
        return;
    }

    auto currentParent = currentGameObject->GetParent();
    if (not currentParent)
    {
        LOG_ERROR << "Root gameObject can not be moved";
        return;
    }

    auto newParent = GetGameObject(event.newParentId);
    if (not newParent)
    {
        LOG_WARN << "Change parent error. New parent not found: " << event.newParentId;
        return;
    }

    const auto& currentWorldTransform = currentGameObject->GetWorldTransform();
    auto worldPosition                = currentWorldTransform.GetPosition();
    auto worldRotation                = currentWorldTransform.GetRotation();
    auto worldScale                   = currentWorldTransform.GetScale();

    auto freeGameObject = currentParent->MoveChild(currentGameObject->GetId());

    if (freeGameObject)
    {
        auto go = freeGameObject.get();
        newParent->MoveChild(std::move(freeGameObject));
        go->SetWorldPositionRotationScale(worldPosition, worldRotation, worldScale);
    }

    NotifySceneEventSubscribers(event);
}

IdType Scene::SubscribeForSceneEvent(std::function<void(const SceneNotifEvent&)> func)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex);
    auto id = eventSubscribersPool.getId();
    eventSubscribers.insert({id, func});
    return id;
}

void Scene::NotifySceneEventSubscribers(const SceneNotifEvent& event)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex);
    for (auto& [_, sub] : eventSubscribers)
        sub(event);
}

void Scene::UnSubscribeForSceneEvent(IdType id)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex);
    eventSubscribers.erase(id);
}

void Scene::AddGameObject(std::unique_ptr<GameObject> object)
{
    LOG_DEBUG << "AddGameObject to queue: " << object->GetName();
    AddGameObjectEvent event{.parentGameObject = rootGameObject_->GetId(), .gameObject = std::move(object)};
    SendEvent(std::move(event));
}

void Scene::AddGameObject(GameObject& parent, std::unique_ptr<GameObject> object)
{
    LOG_DEBUG << "AddGameObject to queue: " << object->GetName();

    if (GetGameObject(parent.GetId()) != nullptr)
    {
        AddGameObjectEvent event{.parentGameObject = parent.GetId(), .gameObject = std::move(object)};
        SendEvent(std::move(event));
    }
    else
    {
        LOG_DEBUG << "Parent object is not added to scene. AddChild directly.";
        parent.AddChild(std::move(object));
    }
}

void Scene::ChangeParent(IdType gameObject, IdType newParent)
{
    SendEvent(ChangeParentEvent{.newParentId = newParent, .gameObjectId = gameObject});
}

void Scene::ChangeParent(GameObject& gameObject, GameObject& newParent)
{
    SendEvent(ChangeParentEvent{.newParentId = newParent.GetId(), .gameObjectId = gameObject.GetId()});
}

void Scene::RemoveParent(GameObject& gameObject)
{
    SendEvent(ChangeParentEvent{.newParentId = rootGameObject_->GetId(), .gameObjectId = gameObject.GetId()});
}

void Scene::RemoveGameObject(IdType id)
{
    SendEvent(RemoveGameObjectEvent{.gameObjectId = id});
}

void Scene::RemoveGameObject(GameObject& object)
{
    SendEvent(RemoveGameObjectEvent{.gameObjectId = object.GetId()});
}

void Scene::ClearGameObjects()
{
    SendEvent(ClearGameObjectsEvent{});
}

GameObject* Scene::GetGameObject(uint32 id) const
{
    LOG_DEBUG << "GetGameObject " << id;

    if (id == 0)
        return rootGameObject_.get();

    if (gameObjectsIds_.count(id))
        return gameObjectsIds_.at(id);

    if (id == 0)
        return rootGameObject_.get();

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

void Scene::SendEvent(SceneEvent&& event)
{
    std::lock_guard<std::mutex> lk(eventsMutex);
    events.push_back(std::move(event));
}

void Scene::SendEvent(EngineEvent& event)
{
    engineContext->AddEngineEvent(event);
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
const WindParams& Scene::getWindParams() const
{
    return windParams;
}
void Scene::setWindParams(const WindParams& params)
{
    windParams = params;
}
}  // namespace GameEngine
