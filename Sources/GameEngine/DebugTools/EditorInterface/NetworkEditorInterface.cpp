#include "NetworkEditorInterface.h"

#include <Input/InputManager.h>
#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <filesystem>

#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/DebugTools/MousePicker/DragObject.h"
#include "GameEngine/DebugTools/MousePicker/MousePicker.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/AvailableComponentMsgInd.h"
#include "Messages/CameraMsg.h"
#include "Messages/ComponentDataMessage.h"
#include "Messages/GameObjectDeleted.h"
#include "Messages/NewComponentMsgInd.h"
#include "Messages/NewGameObjectInd.h"
#include "Messages/RemoveComponentMsgInd.h"
#include "Messages/RemoveGameObjectInd.h"
#include "Messages/SceneFileMsg.h"
#include "Messages/SceneStartedNotifMsg.h"
#include "Messages/SceneStopedNotifMsg.h"
#include "Messages/SelectedObjectChanged.h"
#include "Messages/Transform.h"
#include "Messages/XmlMessageConverter.h"
#include "Messages/GameObjectRenamed.h"

namespace GameEngine
{
namespace
{
std::unique_ptr<FirstPersonCamera> firstPersonCamera;

std::mutex transformChangedMutex_;
std::optional<uint32> transformChangedToSend_;

std::atomic_bool cameraChangedToSend_;
uint64 sendChangeTimeInterval{100};
}  // namespace

NetworkEditorInterface::NetworkEditorInterface(Scene &scene)
    : scene_(scene)
    , transformChangeSubscription_{nullptr}
    , selectedGameObject_{nullptr}
    , userId_{0}
    , keysSubscriptionsManager_(*scene_.inputManager_)
    , running_{false}
{
    SetupCamera();
}

NetworkEditorInterface::~NetworkEditorInterface()
{
    DEBUG_LOG("destructor");

    KeysUnsubscribe();
    UnsubscribeTransformUpdateIfExist();
    UnsubscribeCameraUpdateIfExist();

    EngineContext.threadSync_.Unsubscribe(threadId_);
}

void NetworkEditorInterface::Run()
{
    StopScene();

    if (not running_)
    {
        DefineCommands();
        StartGatway();
        PrepareDebugModels();
    }

    running_ = true;
}
void NetworkEditorInterface::AddObject(const std::string &path)
{
    DEBUG_LOG("AddObject not implemented : path=" + path);
}

void NetworkEditorInterface::DefineCommands()
{
    // clang-format off
    commands_.insert({"openFile", [&](const EntryParameters &v) { LoadSceneFromFile(v); }});
    commands_.insert({"saveToFile", [&](const EntryParameters &v) { SaveSceneToFile(v); }});
    commands_.insert({"getObjectList", [&](const EntryParameters &v) { GetObjectList(v); }});
    commands_.insert({"getRunningStatus", [&](const EntryParameters &v) { GetRunningStatus(v); }});
    commands_.insert({"transformReq", [&](const EntryParameters &v) { TransformReq(v); }});
    commands_.insert({"getGameObjectComponentsListReq", [&](const EntryParameters &v) { GetGameObjectComponentsListReq(v); }});
    commands_.insert({"setPosition", [&](const EntryParameters &v) { SetGameObjectPosition(v); }});
    commands_.insert({"setRotation", [&](const EntryParameters &v) { SetGameObjectRotation(v); }});
    commands_.insert({"setScale", [&](const EntryParameters &v) { SetGameObjectScale(v); }});
    commands_.insert({"createGameObject", [&](const EntryParameters &v) { CreateGameObject(v); }});
    commands_.insert({"deleteGameObject", [&](const EntryParameters& v) { DeleteGameObject(v); }});
    commands_.insert({"renameGameObject", [&](const EntryParameters& v) { RenameGameObject(v);}});
    commands_.insert({"addComponent", [&](const EntryParameters &v) { AddComponent(v); }});
    commands_.insert({"startScene", [&](const EntryParameters &) { StartScene(); }});
    commands_.insert({"stopScene", [&](const EntryParameters &) { StopScene(); }});
    commands_.insert({"getComponentList", [&](const EntryParameters &v) { GetComponentsList(v); }});
    commands_.insert({"getComponentParams", [&](const EntryParameters &v) { GetComponentParams(v); }});
    commands_.insert({"getCamera", [&](const EntryParameters &v) { GetCamera(v); }});
    commands_.insert({"modifyComponentReq", [&](const EntryParameters &v) { ModifyComponentReq(v); }});
    commands_.insert({"createGameObjectWithModel", [&](const EntryParameters &v) { CreateGameObjectWithModel(v); }});
    commands_.insert({"loadPrefab", [&](const EntryParameters& v) { LoadPrefab(v); }});
    commands_.insert({"exit", [&](const EntryParameters&) { scene_.addEngineEvent(EngineEvent(EngineEvent::QUIT)); }});
    gateway_.AddMessageConverter(std::make_unique<DebugNetworkInterface::XmlMessageConverter>());
    // clang-format on
}

void NetworkEditorInterface::SetupCamera()
{
    firstPersonCamera = std::make_unique<FirstPersonCamera>(*scene_.inputManager_, *scene_.displayManager_);
    firstPersonCamera->Lock();
}

void NetworkEditorInterface::StartGatway()
{
    DEBUG_LOG("Starting server");
    gateway_.StartServer(30, 1991);
    gateway_.SetDefaultMessageConverterFormat(Network::MessageFormat::Xml);
    gateway_.SubscribeForNewUser(
        std::bind(&NetworkEditorInterface::NewUser, this, std::placeholders::_1, std::placeholders::_2));
    gateway_.SubscribeForDisconnectUser(
        std::bind(&NetworkEditorInterface::DisconnectUser, this, std::placeholders::_1));
    gateway_.SubscribeOnMessageArrived(
        Network::MessageTypes::Text,
        std::bind(&NetworkEditorInterface::OnMessage, this, std::placeholders::_1, std::placeholders::_2));

    threadId_ = EngineContext.threadSync_.Subscribe(
        [&](float) {
            gateway_.Update();

            NotifIfObjectIsChanged();

            if (selectedGameObject_)
            {
                arrowsIndicatorTransform_.SetPositionAndRotation(selectedGameObject_->worldTransform.GetPosition(),
                                                                 selectedGameObject_->worldTransform.GetRotation());
            }
            if (dragObject_)
                dragObject_->Update();
        },
        "NetworkEditorFps");
}

void NetworkEditorInterface::PrepareDebugModels()
{
    auto model = scene_.resourceManager_->LoadModel("Meshes/Indicators/Arrows.obj");

    if (model)
    {
        scene_.renderersManager_->GetDebugRenderer().AddDebugObject(*model, arrowsIndicatorTransform_);
    }
}

void NetworkEditorInterface::KeysSubscribtions()
{
    float rotationSpeed       = 2.f;
    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [this, rotationSpeed]() {
        if (selectedGameObject_)
        {
            IncreseGameObjectRotation(*selectedGameObject_, GetRotationValueBasedOnKeys(rotationSpeed, 1.f));
        }
    });
    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [this, rotationSpeed]() {
            if (selectedGameObject_)
            {
                IncreseGameObjectRotation(*selectedGameObject_, GetRotationValueBasedOnKeys(rotationSpeed, -1.f));
            }
        });
    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::LCTRL, [&]() {
        if (scene_.camera.IsLocked())
        {
            scene_.camera.Unlock();
        }
        else
        {
            scene_.camera.Lock();
        }
    });
    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        MousePicker mousePicker(scene_.camera, scene_.renderersManager_->GetProjection(), EngineConf.window.size);

        selectedGameObject_ =
            mousePicker.SelectObject(scene_.inputManager_->GetMousePosition(), scene_.GetGameObjects());

        if (selectedGameObject_)
        {
            DEBUG_LOG("selected object : " + selectedGameObject_->GetName());

            dragObject_ = std::make_unique<DragObject>(*scene_.inputManager_, *selectedGameObject_, scene_.camera,
                                                       scene_.renderersManager_->GetProjection());

            if (userId_ > 0)
            {
                DebugNetworkInterface::SelectedObjectChanged msg(selectedGameObject_->GetId());
                gateway_.Send(userId_, msg);
            }
        }
        else
        {
            DEBUG_LOG("no object selected");
        }
    });
    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() { dragObject_ = nullptr; });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(
        KeyCodes::ESCAPE, [this]() { scene_.addEngineEvent(EngineEvent::ASK_QUIT); });
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [this]() { StartScene(); });
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F2, [this]() { StopScene(); });
}

void NetworkEditorInterface::KeysUnsubscribe()
{
    keysSubscriptionsManager_.UnsubscribeKeys();
}

void NetworkEditorInterface::NotifIfObjectIsChanged()
{
    NotifSelectedTransformIsChaned();
    NotifSelectedCameraIsChaned();
}

void NetworkEditorInterface::NotifSelectedTransformIsChaned()
{
    if (transformChangedToSend_ and transformTimer_.GetTimeMiliSeconds() > sendChangeTimeInterval)
    {
        std::lock_guard<std::mutex> lk(transformChangedMutex_);
        auto go               = scene_.GetGameObject(*transformChangedToSend_);
        const auto &transform = go->worldTransform;
        DebugNetworkInterface::Transform msg(*transformChangedToSend_, transform.GetPosition(),
                                             transform.GetRotation().GetEulerDegrees().value, transform.GetScale());
        gateway_.Send(userId_, msg);
        transformChangedToSend_ = std::nullopt;
        transformTimer_.Reset();
    }
}

void NetworkEditorInterface::NotifSelectedCameraIsChaned()
{
    if (cameraChangedToSend_ and cameraTimer_.GetTimeMiliSeconds() > sendChangeTimeInterval)
    {
        DebugNetworkInterface::CameraMsg msg;
        msg.position = scene_.GetCamera().GetPosition();
        msg.rotation = scene_.GetCamera().GetRotation();
        gateway_.Send(userId_, msg);
        cameraChangedToSend_.store(false);
        cameraTimer_.Reset();
    }
}

void NetworkEditorInterface::SetGameObjectPosition(GameObject &gameObject, const vec3 &position)
{
    auto rigidbody = gameObject.GetComponent<Components::Rigidbody>();
    if (rigidbody)
    {
        rigidbody->SetPosition(position);
    }
    gameObject.worldTransform.SetPosition(position);
    gameObject.worldTransform.TakeSnapShoot();
}

void NetworkEditorInterface::SetGameObjectRotation(GameObject &gameObject, const vec3 &rotation)
{
    auto rigidbody = gameObject.GetComponent<Components::Rigidbody>();
    if (rigidbody)
    {
        rigidbody->SetRotation(DegreesVec3(rotation));
    }
    gameObject.worldTransform.SetRotation(DegreesVec3(rotation));
    gameObject.worldTransform.TakeSnapShoot();
}

void NetworkEditorInterface::IncreseGameObjectRotation(GameObject &gameObject, const vec3 &increseValue)
{
    vec3 newValue = gameObject.worldTransform.GetRotation().GetEulerDegrees().value + increseValue;

    auto rigidbody = gameObject.GetComponent<Components::Rigidbody>();
    if (rigidbody)
    {
        rigidbody->SetRotation(DegreesVec3(newValue));
    }

    gameObject.worldTransform.SetRotation(DegreesVec3(newValue));
    gameObject.worldTransform.TakeSnapShoot();
}

vec3 NetworkEditorInterface::GetRotationValueBasedOnKeys(float rotationSpeed, float dir)
{
    vec3 v(0, 0, 0);
    if (scene_.inputManager_->GetKey(KeyCodes::X))
    {
        v.x = dir * rotationSpeed;
    }
    if (scene_.inputManager_->GetKey(KeyCodes::Y))
    {
        v.y = dir * rotationSpeed;
    }
    if (scene_.inputManager_->GetKey(KeyCodes::Z))
    {
        v.z = dir * rotationSpeed;
    }
    return v;
}

void NetworkEditorInterface::NewUser(const std::string &str, uint32 id)
{
    DEBUG_LOG("New user : {" + str + ", " + std::to_string(id) + "}");
    userId_ = id;

    if (not scene_.GetFile().empty())
    {
        DebugNetworkInterface::SceneFileMsg msg(Utils::GetAbsolutePath(scene_.GetFile()));
        gateway_.Send(userId_, msg);
    }
}
void NetworkEditorInterface::DisconnectUser(uint32 id)
{
    userId_ = 0;
    DEBUG_LOG("Disconnect user : {" + std::to_string(id) + "}");
    UnsubscribeTransformUpdateIfExist();
    StartScene();
}
void NetworkEditorInterface::OnMessage(Network::UserId, std::unique_ptr<Network::IMessage> msg)
{
    auto textMsg = Network::castMessageAs<Network::TextMessage>(msg.get())->GetText();
    DEBUG_LOG("Recevied command : " + textMsg);

    auto splitCommand = Utils::SplitString(textMsg, ' ');

    if (splitCommand.empty())
    {
        DEBUG_LOG("splitCommand empty");
        return;
    }

    if (commands_.count(splitCommand[0]))
    {
        commands_.at(splitCommand[0])(CreateParamMap(splitCommand));
    }
    else
    {
        DEBUG_LOG("Unknown command : \"" + splitCommand[0] + "\"");
    }
}
void NetworkEditorInterface::LoadSceneFromFile(const EntryParameters &args)
{
    if (args.count("filename") == 0)
    {
        DEBUG_LOG("Filename not found.");
        return;
    }

    scene_.LoadFromFile(args.at("filename"));
}

void NetworkEditorInterface::SaveSceneToFile(const NetworkEditorInterface::EntryParameters &args)
{
    if (args.count("filename") == 0)
    {
        DEBUG_LOG("Filename not found.");
        return;
    }

    scene_.SaveToFile(args.at("filename"));
}

void NetworkEditorInterface::GetCamera(const EntryParameters &)
{
    UnsubscribeTransformUpdateIfExist();

    DebugNetworkInterface::CameraMsg msg;
    msg.position = scene_.GetCamera().GetPosition();
    msg.rotation = scene_.GetCamera().GetRotation();
    gateway_.Send(userId_, msg);

    cameraChangeSubscriptionId_ =
        scene_.camera.SubscribeOnChange([&](const auto &camera) { cameraChangedToSend_.store(true); });
}

void SendChildrenObjectList(uint32 userId, Network::Gateway &gateway, uint32 parentId,
                            const std::vector<std::unique_ptr<GameObject>> &objectList)
{
    if (objectList.empty())
    {
        DEBUG_LOG("No children found. Parent id : " + std::to_string(parentId));
        return;
    }

    for (auto &go : objectList)
    {
        DebugNetworkInterface::NewGameObjectInd message(go->GetId(), parentId, go->GetName());
        gateway.Send(userId, message);

        const auto &children = go->GetChildrens();
        SendChildrenObjectList(userId, gateway, go->GetId(), children);
    }
}

void NetworkEditorInterface::GetObjectList(const EntryParameters &)
{
    DEBUG_LOG("");

    auto &objectList = scene_.GetGameObjects();
    if (not objectList.empty())
    {
        for (auto &go : objectList)
        {
            DebugNetworkInterface::NewGameObjectInd message(go->GetId(), 0, go->GetName());
            gateway_.Send(userId_, message);
            SendChildrenObjectList(userId_, gateway_, go->GetId(), go->GetChildrens());
        }
    }
}

void NetworkEditorInterface::TransformReq(const EntryParameters &param)
{
    if (not param.count("id"))
        return;

    auto gameObject = GetGameObject(param.at("id"));

    if (not gameObject)
        return;

    UnsubscribeTransformUpdateIfExist();
    UnsubscribeCameraUpdateIfExist();

    auto &transform                = gameObject->worldTransform;
    transformChangeSubscription_   = &transform;
    auto gameObjectId              = gameObject->GetId();
    transformChangeSubscriptionId_ = transform.SubscribeOnChange([this, gameObjectId](const auto &transform) {
        if (not transformChangedToSend_)
        {
            std::lock_guard<std::mutex> lk(transformChangedMutex_);
            transformChangedToSend_ = gameObjectId;
        }
    });

    DebugNetworkInterface::Transform msg(gameObject->GetId(), transform.GetPosition(),
                                         transform.GetRotation().GetEulerDegrees().value, transform.GetScale());
    gateway_.Send(userId_, msg);
}

void NetworkEditorInterface::GetGameObjectComponentsListReq(const EntryParameters &param)
{
    if (not param.count("id"))
        return;

    auto gameObject = GetGameObject(param.at("id"));

    if (not gameObject)
        return;

    for (auto &component : gameObject->GetComponents())
    {
        DebugNetworkInterface::NewComponentMsgInd componentNameMsg(std::to_string(component->GetType()),
                                                                   component->IsActive());
        gateway_.Send(userId_, componentNameMsg);
    }
}

void NetworkEditorInterface::SetGameObjectPosition(const EntryParameters &param)
{
    if (param.count("x") and param.count("y") and param.count("z") and param.count("id"))
    {
        try
        {
            vec3 position(std::stof(param.at("x")), std::stof(param.at("y")), std::stof(param.at("z")));

            if (param.at("id") == "camera")
            {
                scene_.camera.SetPosition(position);
                return;
            }

            auto gameObject = GetGameObject(param.at("id"));
            if (gameObject)
            {
                SetGameObjectPosition(*gameObject, position);
            }
        }
        catch (...)
        {
            ERROR_LOG("Set position error");
        }
    }
}

void NetworkEditorInterface::SetGameObjectRotation(const EntryParameters &param)
{
    if (param.count("x") and param.count("y") and param.count("z") and param.count("id"))
    {
        try
        {
            vec3 rotation(std::stof(param.at("x")), std::stof(param.at("y")), std::stof(param.at("z")));

            if (param.at("id") == "camera")
            {
                scene_.camera.SetRotation(rotation);
                return;
            }

            auto gameObject = GetGameObject(param.at("id"));
            if (gameObject)
            {
                SetGameObjectRotation(*gameObject, rotation);
            }
        }
        catch (...)
        {
            ERROR_LOG("Set rotation error");
        }
    }
}

void NetworkEditorInterface::SetGameObjectScale(const EntryParameters &param)
{
    if (param.count("x") and param.count("y") and param.count("z") and param.count("id"))
    {
        auto gameObject = GetGameObject(param.at("id"));

        if (gameObject)
        {
            try
            {
                vec3 scale(std::stof(param.at("x")), std::stof(param.at("y")), std::stof(param.at("z")));
                gameObject->worldTransform.SetScale(scale);
                gameObject->worldTransform.TakeSnapShoot();
            }
            catch (...)
            {
                ERROR_LOG("Set scale error. Can not convert str to float.");
            }
        }
    }
}

void NetworkEditorInterface::CreateGameObject(const EntryParameters &params)
{
    std::string goName("NewGameObject");
    if (params.count("name"))
    {
        goName = params.at("name");
    }

    auto gameObject = scene_.CreateGameObject(goName);
    DebugNetworkInterface::NewGameObjectInd message(gameObject->GetId(), 0, gameObject->GetName());
    scene_.AddGameObject(gameObject);
    gateway_.Send(userId_, message);
}

void NetworkEditorInterface::DeleteGameObject(const EntryParameters &params)
{
    if (params.count("gameObjectId"))
    {
        auto go = GetGameObject(params.at("gameObjectId"));
        if (go)
        {
            auto id = go->GetId();
            scene_.RemoveGameObject(go);

            DebugNetworkInterface::GameObjectDeleted msg(id);
            gateway_.Send(userId_, msg);
        }
        else
        {
            ERROR_LOG("GameObject not found");
        }
    }
    else
    {
        ERROR_LOG("Incomplete request");
    }
}

void NetworkEditorInterface::RenameGameObject(const EntryParameters &params)
{
    if (params.count("gameObjectId") and params.count("name"))
    {
        auto go = GetGameObject(params.at("gameObjectId"));
        if (go)
        {
            go->SetName(params.at("name"));
            DebugNetworkInterface::GameObjectRenamed msg(go->GetId(), params.at("name"));
            gateway_.Send(userId_, msg);
        }
        else
        {
            ERROR_LOG("GameObject not found");
        }
    }
    else
    {
        ERROR_LOG("Incomplete request");
    }
}

void NetworkEditorInterface::CreateGameObjectWithModel(const NetworkEditorInterface::EntryParameters &params)
{
    vec3 position(0.f);
    try
    {
        if (params.count("frontCamera"))
        {
            position = scene_.camera.GetPosition();
            position += scene_.camera.GetDirection() * 5.f;
        }
        else
        {
            if (params.count("posX"))
            {
                position.x = stof(params.at("posX"));
            }
            if (params.count("posY"))
            {
                position.y = stof(params.at("posY"));
            }
            if (params.count("posZ"))
            {
                position.z = stof(params.at("posZ"));
            }
        }
    }
    catch (...)
    {
        ERROR_LOG("Position parsing stof error");
    }

    vec3 rotationEulerDegrees(0.f);
    try
    {
        if (params.count("rotX"))
        {
            rotationEulerDegrees.x = stof(params.at("rotX"));
        }
        if (params.count("rotY"))
        {
            rotationEulerDegrees.y = stof(params.at("rotY"));
        }
        if (params.count("rotZ"))
        {
            rotationEulerDegrees.z = stof(params.at("rotZ"));
        }
    }
    catch (...)
    {
        ERROR_LOG("Stof error");
    }

    if (params.count("filename"))
    {
        std::string goName = std::filesystem::path(params.at("filename")).filename().stem().string();

        if (params.count("name"))
        {
            goName = params.at("name");
        }

        auto gameObject = scene_.CreateGameObject(goName);
        try
        {
            gameObject->AddComponent<Components::RendererComponent>().AddModel(
                GetRelativeDataPath(params.at("filename")));
            gameObject->worldTransform.SetPosition(position);
            gameObject->worldTransform.SetRotation(DegreesVec3(rotationEulerDegrees));

            DebugNetworkInterface::NewGameObjectInd message(gameObject->GetId(), 0, gameObject->GetName());
            scene_.AddGameObject(gameObject);
            gateway_.Send(userId_, message);
        }
        catch (...)
        {
            ERROR_LOG("Exception caught");
        }
    }
    else
    {
        ERROR_LOG("mandatory param filename not found.");
    }
}

void NetworkEditorInterface::LoadPrefab(const NetworkEditorInterface::EntryParameters &params)
{
    if (params.count("filename"))
    {
        std::string goName = std::filesystem::path(params.at("filename")).filename().stem().string();
        if (params.count("name"))
        {
            goName = params.at("name");
        }

        auto gameObject = scene_.LoadPrefab(GetRelativeDataPath(params.at("filename")), goName);

        if (gameObject)
        {
            auto position = scene_.camera.GetPosition();
            position += scene_.camera.GetDirection() * 5.f;
            gameObject->worldTransform.SetPosition(position);

            DebugNetworkInterface::NewGameObjectInd message(gameObject->GetId(), 0, gameObject->GetName());
            gateway_.Send(userId_, message);
        }
    }
}

void NetworkEditorInterface::GetComponentsList(const EntryParameters &)
{
    for (auto type : Components::GetComponentTypes())
    {
        DebugNetworkInterface::AvailableComponentMsgInd msg(std::to_string(type));
        gateway_.Send(userId_, msg);
    }
}

void NetworkEditorInterface::AddComponent(const EntryParameters &params)
{
    if (params.count("id") > 0 and params.count("name") > 0)
    {
        auto &idParam       = params.at("id");
        auto &componentName = params.at("name");

        auto objectId = std::stoi(idParam);
        auto go       = scene_.GetGameObject(objectId);

        if (not go)
            return;

        auto componentType = Components::from_string(componentName);

        if (componentType)
        {
            auto component = go->AddComponent(*componentType);
            DebugNetworkInterface::NewComponentMsgInd componentNameMsg(componentName, component->IsActive());
            gateway_.Send(userId_, componentNameMsg);
        }
    }
}

void NetworkEditorInterface::GetComponentParams(const EntryParameters &params)
{
    if (not params.count("gameObjectId") or not params.count("name"))
        return;

    auto gameObject = GetGameObject(params.at("gameObjectId"));

    if (not gameObject)
        return;

    auto componentType = Components::from_string(params.at("name"));

    if (not componentType)
        return;

    auto component = gameObject->GetComponent(*componentType);

    if (not component)
        return;

    std::vector<DebugNetworkInterface::Param> componentParams;

    for (auto &p : component->GetParams())
    {
        componentParams.push_back({p.first, p.second.value, p.second.type});
    }

    DebugNetworkInterface::ComponentDataMessage msg(params.at("name"), std::stoi(params.at("gameObjectId")),
                                                    componentParams);
    gateway_.Send(userId_, msg);
}

void NetworkEditorInterface::StartScene()
{
    if (scene_.start_.load())
        return;

    keysSubscriptionsManager_.Clear();
    scene_.inputManager_->StashPopSubscribers();
    SetOrignalCamera();
    scene_.Start();
    gateway_.Send(userId_, DebugNetworkInterface::SceneStartedNotifMsg(scene_.GetName()));
}

void NetworkEditorInterface::StopScene()
{
    if (not scene_.start_.load())
        return;

    scene_.Stop();
    scene_.inputManager_->StashSubscribers();
    KeysSubscribtions();
    SetFreeCamera();
    gateway_.Send(userId_, DebugNetworkInterface::SceneStopedNotifMsg(scene_.GetName()));
}

void NetworkEditorInterface::ModifyComponentReq(const EntryParameters &paramters)
{
    if (not paramters.count("gameObjectId"))
        return;

    auto gameObject = GetGameObject(paramters.at("gameObjectId"));

    if (not gameObject)
        return;

    auto componentType = Components::from_string(paramters.at("componentName"));

    if (not componentType)
        return;

    auto component = gameObject->GetComponent(*componentType);

    if (not component)
        return;

    auto p = paramters;

    auto iter = p.begin();
    while (iter != p.end())
    {
        if (iter->first == "gameObjectId" or iter->first == "componentName")
        {
            iter = p.erase(iter);
        }
        else
        {
            std::replace(iter->second.begin(), iter->second.end(), '%', ' ');
            ++iter;
        }
    }
    component->InitFromParams(p);
}

void NetworkEditorInterface::GetRunningStatus(const NetworkEditorInterface::EntryParameters &)
{
    if (scene_.start_.load())
    {
        gateway_.Send(userId_, DebugNetworkInterface::SceneStartedNotifMsg(scene_.GetName()));
    }
    else
    {
        gateway_.Send(userId_, DebugNetworkInterface::SceneStopedNotifMsg(scene_.GetName()));
    }
}

std::unordered_map<std::string, std::string> NetworkEditorInterface::CreateParamMap(
    const std::vector<std::string> &param)
{
    std::unordered_map<std::string, std::string> v;
    for (const auto &p : param)
    {
        auto [name, value] = GetParamFromCommand(p);
        v.insert({name, value});
    }
    return v;
}

std::tuple<std::string, std::string> NetworkEditorInterface::GetParamFromCommand(const std::string &param)
{
    auto name  = param.substr(0, param.find_first_of('='));
    auto value = param.substr(param.find_first_of('=') + 1);

    return {name, value};
}

GameObject *NetworkEditorInterface::GetGameObject(const std::string &gameObjectId)
{
    try
    {
        auto objectId   = std::stoi(gameObjectId);
        auto gameObject = scene_.GetGameObject(objectId);

        if (gameObject)
        {
            return gameObject;
        }
    }
    catch (...)
    {
        DEBUG_LOG("Invalid convert gameObject str to int.");
    }

    DEBUG_LOG("GameObject not found. " + gameObjectId);
    return nullptr;
}
void NetworkEditorInterface::UnsubscribeTransformUpdateIfExist()
{
    if (transformChangeSubscriptionId_)
    {
        if (not transformChangeSubscription_)
        {
            ERROR_LOG("Somthing went wrong. transformChangeSubscription_ is nullptr");
            return;
        }

        transformChangeSubscription_->UnsubscribeOnChange(*transformChangeSubscriptionId_);
    }
}

void NetworkEditorInterface::UnsubscribeCameraUpdateIfExist()
{
    if (cameraChangeSubscriptionId_)
    {
        scene_.camera.UnsubscribeOnChange(*cameraChangeSubscriptionId_);
    }
}

void NetworkEditorInterface::SetFreeCamera()
{
    sceneCamera_ = scene_.camera.Get();
    sceneCamera_->Lock();
    firstPersonCamera->SetPosition(sceneCamera_->GetPosition());
    firstPersonCamera->SetRotation(sceneCamera_->GetRotation());
    firstPersonCamera->Lock();
    scene_.SetCamera(*firstPersonCamera);
}

void NetworkEditorInterface::SetOrignalCamera()
{
    scene_.SetCamera(*sceneCamera_);
    sceneCamera_->Unlock();
}
}  // namespace GameEngine
