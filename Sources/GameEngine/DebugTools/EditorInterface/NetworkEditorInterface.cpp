#include "NetworkEditorInterface.h"

#include <Input/InputManager.h>
#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <filesystem>

#include "CameraEditor.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/MousePicker/DragObject.h"
#include "GameEngine/DebugTools/MousePicker/MousePicker.h"
#include "GameEngine/DebugTools/Painter/PlantPainter.h"
#include "GameEngine/DebugTools/Painter/TerrainHeightGenerator.h"
#include "GameEngine/DebugTools/Painter/TerrainHeightPainter.h"
#include "GameEngine/DebugTools/Painter/TerrainTexturePainter.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/AvailableComponentMsgInd.h"
#include "Messages/CameraMsg.h"
#include "Messages/ComponentDataMessage.h"
#include "Messages/GameObjectDeleted.h"
#include "Messages/GameObjectRenamed.h"
#include "Messages/NewComponentMsgInd.h"
#include "Messages/NewGameObjectInd.h"
#include "Messages/PlantBrushEnabled.h"
#include "Messages/ReloadScene.h"
#include "Messages/RemoveComponentMsgInd.h"
#include "Messages/RemoveGameObjectInd.h"
#include "Messages/SceneFileMsg.h"
#include "Messages/SceneStartedNotifMsg.h"
#include "Messages/SceneStopedNotifMsg.h"
#include "Messages/SelectedObjectChanged.h"
#include "Messages/TerrainPainterEnabled.h"
#include "Messages/Transform.h"
#include "Messages/XmlMessageConverter.h"

namespace GameEngine
{
namespace
{
using CameraEditorType = CameraEditor;
std::unique_ptr<CameraEditorType> cameraEditor;

std::mutex transformChangedMutex_;
std::optional<uint32> transformChangedToSend_;

std::atomic_bool cameraChangedToSend_;
uint64 sendChangeTimeInterval{100};
}  // namespace

NetworkEditorInterface::NetworkEditorInterface(Scene &scene, Utils::Thread::ThreadSync &threadSync)
    : scene_(scene)
    , threadSync_(threadSync)
    , transformChangeSubscription_{nullptr}
    , selectedGameObject_{nullptr}
    , userId_{0}
    , keysSubscriptionsManager_(*scene_.inputManager_)
    , sceneCamera_{nullptr}
    , running_{false}
    , arrowsIndicator_{nullptr}
{
}

NetworkEditorInterface::~NetworkEditorInterface()
{
    DEBUG_LOG("destructor");
    if (arrowsIndicator_)
    {
        scene_.resourceManager_->ReleaseModel(*arrowsIndicator_);
    }

    UnsubscribeCameraUpdateIfExist();
    SetOrignalCamera();
    cameraEditor.reset();
    KeysUnsubscribe();
    UnsubscribeTransformUpdateIfExist();
    scene_.renderersManager_->GetDebugRenderer().ClearDebugObjects();
    threadSync_.Unsubscribe(threadId_);
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

void NetworkEditorInterface::Main()
{
    gateway_.Update();

    NotifIfObjectIsChanged();

    if (selectedGameObject_)
    {
        arrowsIndicatorTransform_.SetPositionAndRotation(selectedGameObject_->GetWorldTransform().GetPosition(),
                                                         selectedGameObject_->GetWorldTransform().GetRotation());
    }
    {
        std::lock_guard<std::mutex> lk(dragObjectMutex_);
        if (dragObject_)
            dragObject_->Update();
    }

    {
        std::lock_guard<std::mutex> lk(terrainPainterMutex_);
        if (terrainPainter_ and terrainPainterTimer_.GetTimeMiliSeconds() > (1000 / 30))
        {
            terrainPainter_->Paint(scene_.inputManager_->GetMousePosition());
            terrainPainterTimer_.Reset();
        }
    }
}

#define REGISTER_COMMAND(X, Y) commands_.insert({X, [&](const EntryParameters &v) { Y(v); }})

void NetworkEditorInterface::DefineCommands()
{
    REGISTER_COMMAND("openFile", LoadSceneFromFile);
    REGISTER_COMMAND("saveToFile", SaveSceneToFile);
    REGISTER_COMMAND("getObjectList", GetObjectList);
    REGISTER_COMMAND("getRunningStatus", GetRunningStatus);
    REGISTER_COMMAND("transformReq", TransformReq);
    REGISTER_COMMAND("getGameObjectComponentsListReq", GetGameObjectComponentsListReq);
    REGISTER_COMMAND("setPosition", SetGameObjectPosition);
    REGISTER_COMMAND("setRotation", SetGameObjectRotation);
    REGISTER_COMMAND("setScale", SetGameObjectScale);
    REGISTER_COMMAND("createGameObject", CreateGameObject);
    REGISTER_COMMAND("deleteGameObject", DeleteGameObject);
    REGISTER_COMMAND("renameGameObject", RenameGameObject);
    REGISTER_COMMAND("addComponent", AddComponent);
    REGISTER_COMMAND("startScene", StartScene);
    REGISTER_COMMAND("stopScene", StopScene);
    REGISTER_COMMAND("getComponentList", GetComponentsList);
    REGISTER_COMMAND("getComponentParams", GetComponentParams);
    REGISTER_COMMAND("getCamera", GetCamera);
    REGISTER_COMMAND("modifyComponentReq", ModifyComponentReq);
    REGISTER_COMMAND("createGameObjectWithModel", CreateGameObjectWithModel);
    REGISTER_COMMAND("loadPrefab", LoadPrefab);
    REGISTER_COMMAND("reloadScene", ReloadScene);
    REGISTER_COMMAND("clearAll", ClearAll);
    REGISTER_COMMAND("clearAllGameObjects", ClearAllGameObjects);
    REGISTER_COMMAND("setPhysicsVisualization", SetPhysicsVisualization);
    REGISTER_COMMAND("setNormalsVisualization", SetNormalsVisualization);
    REGISTER_COMMAND("setLineRenderMode", SetLineRenderMode);
    REGISTER_COMMAND("selectGameObject", SelectGameObject);
    REGISTER_COMMAND("goCameraToObject", GoCameraToObject);
    REGISTER_COMMAND("enableTerrainHeightPainter", EnableTerrainHeightPainter);
    REGISTER_COMMAND("enableTerrainTexturePainter", EnableTerrainTexturePainter);
    REGISTER_COMMAND("enablePlantPainter", EnablePlantPainter);
    REGISTER_COMMAND("disablePainter", DisableTerrainPainter);
    REGISTER_COMMAND("updateTerrainPainterParam", UpdateTerrainPainterParam);
    REGISTER_COMMAND("recalculateTerrainNormals", RecalculateTerrainNormals);
    REGISTER_COMMAND("clearTerrainsBlendMap", ClearTerrainsBlendMap);
    REGISTER_COMMAND("controlTextureUsage", ControlTextureUsage);
    REGISTER_COMMAND("generateTerrains", GenerateTerrains);
    REGISTER_COMMAND("reloadShaders", ReloadShaders);
    REGISTER_COMMAND("takeSnapshot", Takesnapshot);
    REGISTER_COMMAND("exit", Exit);

    gateway_.AddMessageConverter(std::make_unique<DebugNetworkInterface::XmlMessageConverter>());
}

#undef REGISTER_COMMAND

void NetworkEditorInterface::SetupCamera()
{
    sceneCamera_ = scene_.camera.Get();
    sceneCamera_->Lock();

    cameraEditor = std::make_unique<CameraEditorType>(*scene_.inputManager_, *scene_.displayManager_);
    cameraEditor->SetPosition(sceneCamera_->GetPosition());
    cameraEditor->SetRotation(sceneCamera_->GetRotation());
    scene_.SetCamera(*cameraEditor);
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

    threadId_ = threadSync_.Subscribe([&](float) { Main(); }, "NetworkEditorFps");
}

void NetworkEditorInterface::PrepareDebugModels()
{
    arrowsIndicator_ = scene_.resourceManager_->LoadModel("Meshes/Indicators/Arrows.obj");

    if (arrowsIndicator_)
    {
        scene_.renderersManager_->GetDebugRenderer().AddDebugObject(*arrowsIndicator_, arrowsIndicatorTransform_);
    }
}

void NetworkEditorInterface::KeysSubscribtions()
{
    float rotationSpeed       = 2.f;
    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [this, rotationSpeed]() {
        if (selectedGameObject_)
        {
            IncreseGameObjectRotation(*selectedGameObject_, GetRotationValueBasedOnKeys(rotationSpeed, 1.f));

            auto moveVector = GetPositionChangeValueBasedOnKeys(0.1f, 1.f);
            moveVector      = selectedGameObject_->GetTransform().GetRotation().value_ * moveVector;
            moveVector      = moveVector + selectedGameObject_->GetTransform().GetPosition();
            selectedGameObject_->GetTransform().SetPosition(moveVector);
        }
    });
    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [this, rotationSpeed]() {
            if (selectedGameObject_)
            {
                IncreseGameObjectRotation(*selectedGameObject_, GetRotationValueBasedOnKeys(rotationSpeed, -1.f));

                auto moveVector = GetPositionChangeValueBasedOnKeys(0.1f, -1.f);
                moveVector      = selectedGameObject_->GetTransform().GetRotation().value_ * moveVector;
                moveVector      = moveVector + selectedGameObject_->GetTransform().GetPosition();
                selectedGameObject_->GetTransform().SetPosition(moveVector);
            }
        });
    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        MousePicker mousePicker(scene_.camera, scene_.renderersManager_->GetProjection(), EngineConf.window.size);

        std::optional<uint32> lastSelectedGameObject;

        if (selectedGameObject_)
        {
            lastSelectedGameObject = selectedGameObject_->GetId();
        }

        selectedGameObject_ =
            mousePicker.SelectObject(scene_.inputManager_->GetMousePosition(), scene_.GetGameObjects());

        if (selectedGameObject_)
        {
            DEBUG_LOG("selected object : " + selectedGameObject_->GetName());

            dragObject_ = std::make_unique<DragObject>(*scene_.inputManager_, *selectedGameObject_, scene_.camera,
                                                       scene_.renderersManager_->GetProjection());

            if (not lastSelectedGameObject or *lastSelectedGameObject != selectedGameObject_->GetId())
            {
                if (userId_ > 0)
                {
                    DebugNetworkInterface::SelectedObjectChanged msg(selectedGameObject_->GetId());
                    gateway_.Send(userId_, msg);
                }
            }
        }
        else
        {
            DEBUG_LOG("no object selected");
        }
    });
    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() {
        {
            std::lock_guard<std::mutex> lk(dragObjectMutex_);
            dragObject_.reset(nullptr);
        }
    });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(
        KeyCodes::ESCAPE, [this]() { scene_.addEngineEvent(EngineEvent::ASK_QUIT); });

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F1,
                                             [this]() { scene_.inputManager_->AddEvent([&]() { StartScene(); }); });

    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::G, [this]() { GenerateTerrainBlendMapToFile(); });
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
        const auto &transform = go->GetTransform();
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
        msg.rotation = scene_.GetCamera().GetRotation().GetEulerDegrees().value;
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
    gameObject.GetTransform().SetPosition(position);
}

void NetworkEditorInterface::SetGameObjectRotation(GameObject &gameObject, const vec3 &rotation)
{
    auto rigidbody = gameObject.GetComponent<Components::Rigidbody>();
    if (rigidbody)
    {
        rigidbody->SetRotation(DegreesVec3(rotation));
    }
    gameObject.GetTransform().SetRotation(DegreesVec3(rotation));
}

void NetworkEditorInterface::IncreseGameObjectRotation(GameObject &gameObject, const vec3 &increseValue)
{
    vec3 newValue = gameObject.GetTransform().GetRotation().GetEulerDegrees().value + increseValue;

    auto rigidbody = gameObject.GetComponent<Components::Rigidbody>();
    if (rigidbody)
    {
        rigidbody->SetRotation(DegreesVec3(newValue));
    }

    gameObject.GetTransform().SetRotation(DegreesVec3(newValue));
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

vec3 NetworkEditorInterface::GetPositionChangeValueBasedOnKeys(float speed, float dir)
{
    vec3 v(0, 0, 0);
    if (scene_.inputManager_->GetKey(KeyCodes::B))
    {
        v.x = dir * speed;
    }
    if (scene_.inputManager_->GetKey(KeyCodes::N))
    {
        v.y = dir * speed;
    }
    if (scene_.inputManager_->GetKey(KeyCodes::M))
    {
        v.z = dir * speed;
    }
    return v;
}

void NetworkEditorInterface::NewUser(const std::string &str, uint32 id)
{
    DEBUG_LOG("New user : {" + str + ", " + std::to_string(id) + "}");
    userId_ = id;

    if (not scene_.GetFile().empty())
    {
        DebugNetworkInterface::SceneFileMsg msg(scene_.GetFile().GetAbsoultePath());
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

    auto command = splitCommand[0];
    splitCommand.erase(splitCommand.begin());

    if (commands_.count(command))
    {
        commands_.at(command)(CreateParamMap(splitCommand));
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
    msg.rotation = scene_.GetCamera().GetRotation().GetEulerDegrees().value;
    gateway_.Send(userId_, msg);

    cameraChangeSubscriptionId_ =
        scene_.camera.SubscribeOnChange([&](const auto &) { cameraChangedToSend_.store(true); });
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

        const auto &children = go->GetChildren();
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
            SendChildrenObjectList(userId_, gateway_, go->GetId(), go->GetChildren());
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

    auto &transform                = gameObject->GetTransform();
    transformChangeSubscription_   = &transform;
    auto gameObjectId              = gameObject->GetId();
    transformChangeSubscriptionId_ = transform.SubscribeOnChange([gameObjectId](const auto &) {
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
                scene_.camera.SetRotation(DegreesVec3(rotation));
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
                gameObject->GetTransform().SetScale(scale);
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
    auto parentId = AddGameObject(params, gameObject);
    if (parentId)
    {
        message.parentId = *parentId;
    }
    gateway_.Send(userId_, message);
}

void NetworkEditorInterface::DeleteGameObject(const EntryParameters &params)
{
    if (params.count("gameObjectId"))
    {
        auto go = GetGameObject(params.at("gameObjectId"));
        if (go)
        {
            UnsubscribeTransformUpdateIfExist();

            auto id = go->GetId();
            scene_.RemoveGameObject(*go);

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
            gameObject->GetTransform().SetPosition(position);
            gameObject->GetTransform().SetRotation(DegreesVec3(rotationEulerDegrees));

            DebugNetworkInterface::NewGameObjectInd message(gameObject->GetId(), 0, gameObject->GetName());
            auto parentId = AddGameObject(params, gameObject);
            if (parentId)
            {
                message.parentId = *parentId;
            }
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
            gameObject->GetTransform().SetPosition(position);

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
            component->ReqisterFunctions();
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

void NetworkEditorInterface::SetDeubgRendererState(DebugRenderer::RenderState state, const EntryParameters &params)
{
    bool set = scene_.renderersManager_->GetDebugRenderer().IsStateEnabled(state);
    if (params.count("enabled"))
    {
        set = Utils::StringToBool(params.at("enabled"));
    }
    else
    {
        set = !set;
    }

    auto &debugRenderer = scene_.renderersManager_->GetDebugRenderer();
    set ? debugRenderer.AddState(state) : debugRenderer.RemoveState(state);
}

void NetworkEditorInterface::SetPhysicsVisualization(const EntryParameters &params)
{
    SetDeubgRendererState(DebugRenderer::RenderState::Physics, params);
}

void NetworkEditorInterface::SetNormalsVisualization(const NetworkEditorInterface::EntryParameters &params)
{
    SetDeubgRendererState(DebugRenderer::RenderState::Normals, params);
}

void NetworkEditorInterface::SetLineRenderMode(const EntryParameters &params)
{
    bool set = scene_.renderersManager_->getLineRenderMode();

    if (params.count("enabled"))
    {
        set = Utils::StringToBool(params.at("enabled"));
    }
    else
    {
        set = !set;
    }
    scene_.renderersManager_->setLineRenderMode(set);
}

void NetworkEditorInterface::SelectGameObject(const EntryParameters &paramters)
{
    if (not paramters.count("gameObjectId"))
        return;

    auto gameObject = GetGameObject(paramters.at("gameObjectId"));

    if (not gameObject)
        return;

    selectedGameObject_ = gameObject;
}

void NetworkEditorInterface::GoCameraToObject(const NetworkEditorInterface::EntryParameters &paramters)
{
    if (not paramters.count("gameObjectId"))
        return;

    auto gameObject = GetGameObject(paramters.at("gameObjectId"));

    if (not gameObject)
        return;

    cameraEditor->SetPosition(gameObject->GetWorldTransform().GetPosition() +
                              (3.f * gameObject->GetWorldTransform().GetScale()));
    cameraEditor->LookAt(gameObject->GetWorldTransform().GetPosition());
}

void NetworkEditorInterface::StartScene(const NetworkEditorInterface::EntryParameters &)
{
    StartScene();
}

void NetworkEditorInterface::StopScene(const NetworkEditorInterface::EntryParameters &)
{
    StopScene();
}

void NetworkEditorInterface::StartScene()
{
    if (scene_.start_.load())
        return;

    scene_.renderersManager_->GetDebugRenderer().Disable();
    keysSubscriptionsManager_.Clear();
    SetOrignalCamera();
    scene_.inputManager_->StashPopSubscribers();

    scene_.Start();
    gateway_.Send(userId_, DebugNetworkInterface::SceneStartedNotifMsg(scene_.GetName()));
}

void NetworkEditorInterface::StopScene()
{
    if (not scene_.start_.load())
        return;

    UnsubscribeTransformUpdateIfExist();
    scene_.Stop();

    scene_.inputManager_->StashSubscribers();
    SetupCamera();
    KeysSubscribtions();
    scene_.renderersManager_->GetDebugRenderer().Enable();

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

void NetworkEditorInterface::ReloadScene(const EntryParameters &)
{
    DebugNetworkInterface::ReloadScene msg(scene_.GetName());
    gateway_.Send(userId_, msg);
    scene_.addSceneEvent(SceneEventType::RELOAD_SCENE);
}

void NetworkEditorInterface::ClearAll(const EntryParameters &v)
{
    ClearAllGameObjects(v);
    // ClearLigts(v);
    // ClearGui();
}

void NetworkEditorInterface::ClearAllGameObjects(const EntryParameters &)
{
    for (const auto &go : scene_.GetGameObjects())
    {
        // If parent is erase child automatically too
        DebugNetworkInterface::GameObjectDeleted msg(go->GetId());
        gateway_.Send(userId_, msg);
    }
    scene_.ClearGameObjects();
}

DebugNetworkInterface::TerrainPainterEnabled PrepareTerrainPainterEnabledMsg(Painter &painter)
{
    DebugNetworkInterface::TerrainPainterEnabled msg;
    msg.type               = std::to_string(painter.GetPaintType());
    msg.strength           = painter.strength_;
    msg.brushSize          = painter.brushSize_;
    msg.selectedBrushType  = painter.SelectedBrush();
    msg.stepInterpolation  = std::to_string(painter.stepInterpolation_);
    msg.stepInterpolations = AvaiableStepInterpolationsStrs();
    msg.brushTypes         = painter.AvaiableBrushTypes();

    return msg;
}

Painter::EntryParamters NetworkEditorInterface::GetPainterEntryParameters()
{
    return Painter::EntryParamters{*scene_.inputManager_, scene_.camera, scene_.renderersManager_->GetProjection(),
                                   scene_.displayManager_->GetWindowSize(), scene_.componentController_};
}

void NetworkEditorInterface::EnableTerrainHeightPainter(const EntryParameters &)
{
    DisableTerrainPainter({});
    terrainPainter_ = std::make_unique<TerrainHeightPainter>(GetPainterEntryParameters());
    gateway_.Send(userId_, PrepareTerrainPainterEnabledMsg(*terrainPainter_));
}

void NetworkEditorInterface::EnableTerrainTexturePainter(const EntryParameters &)
{
    DisableTerrainPainter({});
    terrainPainter_ = std::make_unique<TerrainTexturePainter>(GetPainterEntryParameters(), Color(255, 0, 0));
    gateway_.Send(userId_, PrepareTerrainPainterEnabledMsg(*terrainPainter_));
}

void NetworkEditorInterface::EnablePlantPainter(const EntryParameters &params)
{
    if (not params.count("gameObjectId"))
    {
        ERROR_LOG("GameObject Id is necessary to get plant component.");
        return;
    }

    auto gameObject = GetGameObject(params.at("gameObjectId"));

    if (gameObject)
    {
        auto component = gameObject->GetComponent<Components::GrassRendererComponent>();

        if (not component)
        {
            ERROR_LOG("Grass component not found in object : " + params.at("gameObjectId"));
            return;
        }

        DisableTerrainPainter({});
        terrainPainter_ = std::make_unique<PlantPainter>(GetPainterEntryParameters(), *component);
        gateway_.Send(userId_, PrepareTerrainPainterEnabledMsg(*terrainPainter_));
    }
}

void NetworkEditorInterface::DisableTerrainPainter(const EntryParameters &)
{
    if (not terrainPainter_)
    {
        return;
    }

    std::lock_guard<std::mutex> lk(terrainPainterMutex_);
    terrainPainter_.reset(nullptr);
}

void NetworkEditorInterface::UpdateTerrainPainterParam(const NetworkEditorInterface::EntryParameters &params)
{
    std::lock_guard<std::mutex> lk(terrainPainterMutex_);
    if (not terrainPainter_)
        return;

    try
    {
        if (params.count("strength"))
        {
            terrainPainter_->strength_ = std::stof(params.at("strength"));
        }
        if (params.count("brushSize"))
        {
            terrainPainter_->brushSize_ = std::stoi(params.at("brushSize"));
        }
        if (params.count("stepInterpolation"))
        {
            std::from_string(params.at("stepInterpolation"), terrainPainter_->stepInterpolation_);
        }
        if (params.count("brushType"))
        {
            terrainPainter_->SetBrush(params.at("brushType"));
        }
        if (params.count("color"))
        {
            if (terrainPainter_->GetPaintType() != PaintType::BlendMap)
            {
                ERROR_LOG("Incompatible paint mode.");
                return;
            }

            auto inputColor = params.at("color");
            size_t i        = 0;
            Color color;
            for (auto c : inputColor)
            {
                if (i < 4)
                    color[i++] = c == '1' ? 1.f : 0.f;
                else
                    ERROR_LOG("to many bits.");
            }

            static_cast<TerrainTexturePainter *>(terrainPainter_.get())->SetColor(color);
        }
    }
    catch (...)
    {
        ERROR_LOG("Message parsing error.");
    }
}

void NetworkEditorInterface::RecalculateTerrainNormals(const NetworkEditorInterface::EntryParameters &)
{
    std::lock_guard<std::mutex> lk(terrainPainterMutex_);
    if (not terrainPainter_ and terrainPainter_->GetPaintType() == PaintType::HeightMap)
        return;

    static_cast<TerrainHeightPainter *>(terrainPainter_.get())->RecalculateTerrainNormals();
}
void NetworkEditorInterface::ClearTerrainsBlendMap(const EntryParameters &)
{
    auto terrains = scene_.componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainRenderer);

    for (auto &terrain : terrains)
    {
        auto tc = static_cast<Components::TerrainRendererComponent *>(terrain.second);

        if (not tc)
            continue;

        auto image    = CreateZerosImage<uint8>(vec2ui(4096, 4096), 4);
        auto blendMap = static_cast<GeneralTexture *>(tc->GetTexture(TerrainTextureType::blendMap));
        blendMap->SetImage(std::move(image));
        tc->BlendMapChanged();
    }
}

void NetworkEditorInterface::GenerateTerrains(const EntryParameters &params)
{
    TerrainHeightGenerator::EntryParamters entryParamters;
    bool updateNoiseSeed{true};

    try
    {
        if (params.count("bias"))
        {
            entryParamters.bias = std::stof(params.at("bias"));
        }
        if (params.count("octaves"))
        {
            entryParamters.octaves = std::stoi(params.at("octaves"));
        }
        if (params.count("width"))
        {
            entryParamters.perTerrainHeightMapsize.x = std::stoi(params.at("width"));
        }
        if (params.count("height"))
        {
            entryParamters.perTerrainHeightMapsize.y = std::stoi(params.at("height"));
        }
        if (params.count("scale"))
        {
            entryParamters.scale = std::stof(params.at("scale"));
        }
        if (params.count("heightFactor"))
        {
            entryParamters.heightFactor = std::stof(params.at("heightFactor"));
        }
        if (params.count("updateNoiseSeed"))
        {
            updateNoiseSeed = Utils::StringToBool(params.at("updateNoiseSeed"));
        }
    }
    catch (...)
    {
        ERROR_LOG("Pram parsing failed.");
    }

    TerrainHeightGenerator generator(scene_.componentController_, entryParamters);

    if (updateNoiseSeed)
    {
        generator.generateNoiseSeed();
    }

    generator.generateHeightMapsImage();
}

void NetworkEditorInterface::ControlTextureUsage(const NetworkEditorInterface::EntryParameters &params)
{
    if (not params.count("enabled") or not params.count("textureType"))
        return;

    auto enabled     = Utils::StringToBool(params.at("enabled"));
    auto textureType = params.at("textureType");

    auto &textConf = EngineConf.renderer.textures;

    if (textureType == "diffuse")
    {
        textConf.useDiffuse = enabled;
    }
    else if (textureType == "normal")
    {
        textConf.useNormal = enabled;
    }
    else if (textureType == "specular")
    {
        textConf.useSpecular = enabled;
    }
    else if (textureType == "displacement")
    {
        textConf.useDisplacement = enabled;
    }

    scene_.resourceManager_->GetGpuResourceLoader().AddFunctionToCall(
        [&]() { scene_.renderersManager_->UpdatePerAppBuffer(); });
}

void NetworkEditorInterface::GenerateTerrainBlendMapToFile()
{
    auto terrains = scene_.componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainRenderer);

    std::set<std::string> generetedBlendMaps;

    for (auto &terrain : terrains)
    {
        auto tc = static_cast<Components::TerrainRendererComponent *>(terrain.second);

        if (not tc)
            continue;

        const auto &heightMap = *tc->GetHeightMap();

        auto heightMapFile = *tc->GetTexture(TerrainTextureType::heightmap)->GetFile();

        if (not generetedBlendMaps.count(heightMapFile.GetAbsoultePath()))
        {
            heightMapFile.AddSuffixToBaseName("_generatedBlendmap");

            GenerateBlendMap(tc->GetTerrainConfiguration().GetScale(), heightMap, heightMapFile);
            generetedBlendMaps.insert(heightMapFile.GetAbsoultePath());
        }
    }
}

void NetworkEditorInterface::ReloadShaders(const NetworkEditorInterface::EntryParameters &)
{
    scene_.renderersManager_->ReloadShaders();
}

void NetworkEditorInterface::Takesnapshot(const NetworkEditorInterface::EntryParameters &params)
{
    std::string path{"./snapshoot/"};
    if (params.count("location"))
    {
        path = params.at("location");
    }

    auto takeSnapshoot = [this, path]() { scene_.resourceManager_->GetGraphicsApi().TakeSnapshoot(path); };
    scene_.resourceManager_->GetGpuResourceLoader().AddFunctionToCall(takeSnapshoot);
}

void NetworkEditorInterface::Exit(const NetworkEditorInterface::EntryParameters &)
{
    scene_.addEngineEvent(EngineEvent(EngineEvent::QUIT));
}

NetworkEditorInterface::EntryParameters NetworkEditorInterface::CreateParamMap(const std::vector<std::string> &param)
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
        auto gameObject = scene_.GetGameObject(static_cast<uint32>(objectId));

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
        transformChangeSubscription_   = nullptr;
        transformChangeSubscriptionId_ = std::nullopt;
    }
}

void NetworkEditorInterface::UnsubscribeCameraUpdateIfExist()
{
    if (cameraChangeSubscriptionId_)
    {
        scene_.camera.UnsubscribeOnChange(*cameraChangeSubscriptionId_);
        if (cameraEditor)
            cameraEditor->UnsubscribeOnChange(*cameraChangeSubscriptionId_);
        cameraChangeSubscriptionId_ = std::nullopt;
    }
}

void NetworkEditorInterface::SetOrignalCamera()
{
    scene_.SetCamera(*sceneCamera_);
    if (sceneCamera_)
        sceneCamera_->Unlock();
    cameraEditor.reset();
}
std::optional<uint32> NetworkEditorInterface::AddGameObject(const EntryParameters &params,
                                                            std::unique_ptr<GameObject> &gameObject)
{
    std::optional<uint32> result{std::nullopt};
    if (params.count("parentGameObjectId"))
    {
        auto parentGameObject = GetGameObject(params.at("parentGameObjectId"));

        if (parentGameObject)
        {
            result = parentGameObject->GetId();
            parentGameObject->AddChild(std::move(gameObject));
        }
        else
        {
            ERROR_LOG("Parent not found, parentId=" + params.at("parentGameObjectId"));
        }
    }
    else
    {
        scene_.AddGameObject(std::move(gameObject));
    }
    return result;
}
}  // namespace GameEngine
