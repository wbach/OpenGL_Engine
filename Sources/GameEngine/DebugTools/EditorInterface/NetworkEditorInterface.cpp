#include "NetworkEditorInterface.h"

#include <Input/InputManager.h>
#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>

#include "CameraEditor.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/PreviewComponent.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/MousePicker/DragObject.h"
#include "GameEngine/DebugTools/MousePicker/MousePicker.h"
#include "GameEngine/DebugTools/Painter/PlantPainter.h"
#include "GameEngine/DebugTools/Painter/TerrainHeightGenerator.h"
#include "GameEngine/DebugTools/Painter/TerrainHeightPainter.h"
#include "GameEngine/DebugTools/Painter/TerrainTexturePainter.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Scene/SceneDef.h"
#include "GameEngine/Scene/SceneReader.h"
#include "GameEngine/Scene/SceneUtils.h"
#include "Logger/Log.h"
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

std::mutex transformChangedMutex_;
std::optional<uint32> transformChangedToSend_;

std::atomic_bool cameraChangedToSend_;
uint64 sendChangeTimeInterval{100};

const std::string ROTATE_OBJECT_X{"ROTATE_OBJECT_X"};
const std::string ROTATE_OBJECT_Y{"ROTATE_OBJECT_Y"};
const std::string ROTATE_OBJECT_Z{"ROTATE_OBJECT_Z"};

const std::string MOVE_OBJECT{"MOVE_OBJECT"};
const std::string MOVE_OBJECT_X{"MOVE_OBJECT_X"};
const std::string MOVE_OBJECT_Y{"MOVE_OBJECT_Y"};
const std::string MOVE_OBJECT_Z{"MOVE_OBJECT_Z"};

const std::string SCALE_OBJECT{"SCALE_OBJECT"};
const std::string SCALE_OBJECT_X{"SCALE_OBJECT_X"};
const std::string SCALE_OBJECT_Y{"SCALE_OBJECT_Y"};
const std::string SCALE_OBJECT_Z{"SCALE_OBJECT_Z"};

const std::string DELETE_GAMEOBJECT{"DELETE_GAMEOBJECT"};

const std::string START_SCENE{"START_SCENE"};
const std::string BLENDMAPS_TO_FILE{"BLENDMAPS_TO_FILE"};

const std::string SELECT_OBJECT{"SELECT_OBJECT"};
const std::string OBJECT_CONTROL{"OBJECT_CONTROL"};
const std::string QUICK_SAVE{"QUICK_SAVE"};
const std::string EXIT{"EXIT"};

// clang-format off
std::unordered_map<std::string, KeyCodes::Type> editorActions
{
    {ROTATE_OBJECT_X, KeyCodes::X},
    {ROTATE_OBJECT_Y, KeyCodes::Y},
    {ROTATE_OBJECT_Z, KeyCodes::Z},

    {MOVE_OBJECT, KeyCodes::G},
    {MOVE_OBJECT_X, KeyCodes::B},
    {MOVE_OBJECT_Y, KeyCodes::N},
    {MOVE_OBJECT_Z, KeyCodes::M},

    {SCALE_OBJECT, KeyCodes::H},
    {SCALE_OBJECT_X, KeyCodes::J},
    {SCALE_OBJECT_Y, KeyCodes::K},
    {SCALE_OBJECT_Z, KeyCodes::L},

    {DELETE_GAMEOBJECT, KeyCodes::DEL},

    {QUICK_SAVE, KeyCodes::F2},
    {START_SCENE, KeyCodes::F9},
    {BLENDMAPS_TO_FILE, KeyCodes::F5},

    {SELECT_OBJECT, KeyCodes::LMOUSE},
    {OBJECT_CONTROL, KeyCodes::MOUSE_WHEEL},
    {SELECT_OBJECT, KeyCodes::LMOUSE},
    {OBJECT_CONTROL, KeyCodes::MOUSE_WHEEL},
    {EXIT, KeyCodes::ESCAPE}
};
// clang-format on
}  // namespace

NetworkEditorInterface::NetworkEditorInterface(Scene &scene, Utils::Thread::ThreadSync &threadSync)
    : scene_(scene)
    , threadSync_(threadSync)
    , transformChangeSubscription_{nullptr}
    , selectedGameObject_{nullptr}
    , userId_{0}
    , keysSubscriptionsManager_(*scene_.inputManager_)
    , running_{false}
    , arrowsIndicator_{nullptr}
{
}

NetworkEditorInterface::~NetworkEditorInterface()
{
    if (arrowsIndicator_)
    {
        scene_.resourceManager_->ReleaseModel(*arrowsIndicator_);
    }

    UnsubscribeCameraUpdateIfExist();
    SetOrignalCamera();
    KeysUnsubscribe();
    UnsubscribeTransformUpdateIfExist();
    scene_.renderersManager_->GetDebugRenderer().clear();
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
    LOG_ERROR << "AddObject not implemented : path=" << path;
}

void NetworkEditorInterface::MainLoop()
{
    gateway_.Update();
    NotifIfObjectIsChanged();
    UpdateArrowsIndicatorPosition();
    UpdateDragObject();
    PaintTerrain();
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
    REGISTER_COMMAND("setPhysicsVisualizationAllObjects", SetPhysicsVisualizationAllObjcts);
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
    REGISTER_COMMAND("moveObjectToCameraPosition", MoveObjectToCameraPosition);
    REGISTER_COMMAND("modelPreviewRequest", ModelPreviewRequest);
    REGISTER_COMMAND("changeGameObjectParent", ChangeGameObjectParent);
    REGISTER_COMMAND("cloneGameObject", CloneGameObject);
    REGISTER_COMMAND("cloneGameObjectInstancesWithRandomPosition", CloneGameObjectInstancesWithRandomPosition);
    REGISTER_COMMAND("createPrefabFromObject", CreatePrefabFromObject);
    REGISTER_COMMAND("createTerrainTransition", CreateTerrainTranstion);

    gateway_.AddMessageConverter(std::make_unique<DebugNetworkInterface::XmlMessageConverter>());
}

#undef REGISTER_COMMAND

void NetworkEditorInterface::SetupCamera()
{
    if (cameraEditorId_)
    {
    }
    scene_.camera.Lock();

    auto cameraEditor = std::make_unique<CameraEditorType>(*scene_.inputManager_, *scene_.displayManager_);
    cameraEditor->SetPosition(scene_.camera.GetPosition());
    cameraEditor->SetRotation(scene_.camera.GetRotation());
    scene_.inputManager_->ShowCursor(true);
    scene_.inputManager_->SetReleativeMouseMode(false);
    cameraEditorId_ = scene_.camera.addAndSet(std::move(cameraEditor));
}

void NetworkEditorInterface::StartGatway()
{
    LOG_DEBUG << "Starting server";
    gateway_.StartServer(30, 1991);
    gateway_.SetDefaultMessageConverterFormat(Network::MessageFormat::Xml);
    gateway_.SubscribeForNewUser(std::bind(&NetworkEditorInterface::NewUser, this, std::placeholders::_1, std::placeholders::_2));
    gateway_.SubscribeForDisconnectUser(std::bind(&NetworkEditorInterface::DisconnectUser, this, std::placeholders::_1));
    gateway_.SubscribeOnMessageArrived(Network::MessageTypes::Text, std::bind(&NetworkEditorInterface::OnMessage, this,
                                                                              std::placeholders::_1, std::placeholders::_2));

    threadId_ = threadSync_.Subscribe([&](float) { MainLoop(); }, "NetworkEditorFps");
}

void NetworkEditorInterface::PrepareDebugModels()
{
    arrowsIndicator_ = scene_.resourceManager_->LoadModel("Meshes/Indicators/Arrows.obj");

    if (arrowsIndicator_)
    {
        arrowsIndicatorTransform_.SetPosition(vec3(std::numeric_limits<float>::max()));
        scene_.renderersManager_->GetDebugRenderer().AddDebugObject(*arrowsIndicator_, arrowsIndicatorTransform_);
    }

    brushCircle_ = scene_.resourceManager_->LoadModel("Meshes/Indicators/circle2.obj");

    if (brushCircle_)
    {
        brushCircleTransform_.SetPosition(vec3(std::numeric_limits<float>::max()));
        scene_.renderersManager_->GetDebugRenderer().AddDebugObject(*brushCircle_, brushCircleTransform_);
    }
}

void NetworkEditorInterface::KeysSubscribtions()
{
    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyUp(editorActions.at(OBJECT_CONTROL), [this]() { ObjectControlAction(1.f); });
    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyDown(editorActions.at(OBJECT_CONTROL), [this]() { ObjectControlAction(-1.f); });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(
        editorActions.at(MOVE_OBJECT),
        [this]() { UseSelectedGameObject([this](auto &gameobject) { CreateDragObject(gameobject); }); });

    keysSubscriptionsManager_ =
        scene_.inputManager_->SubscribeOnKeyUp(editorActions.at(MOVE_OBJECT), [this]() { ReleaseDragObject(); });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(
        editorActions.at(SELECT_OBJECT),
        [this]()
        {
            MousePicker mousePicker(scene_.camera, scene_.renderersManager_->GetProjection());

            SetSelectedGameObject(mousePicker.SelectObject(scene_.inputManager_->GetMousePosition(), scene_.GetGameObjects()));
        });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(
        editorActions.at(EXIT), [this]() { scene_.engineContext->AddEngineEvent(QuitEvent::ASK_QUIT); });

    scene_.inputManager_->SubscribeOnKeyDown(editorActions.at(START_SCENE),
                                             [this]() { scene_.inputManager_->AddEvent([&]() { StartScene(); }); });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(editorActions.at(BLENDMAPS_TO_FILE),
                                                                         [this]() { GenerateTerrainBlendMapToFile(); });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(editorActions.at(QUICK_SAVE), [this]() { QuickSave(); });

    keysSubscriptionsManager_ = scene_.inputManager_->SubscribeOnKeyDown(editorActions.at(DELETE_GAMEOBJECT),
                                                                         [this]()
                                                                         {
                                                                             if (selectedGameObject_)
                                                                             {
                                                                                 DeleteGameObject(*selectedGameObject_);
                                                                             }
                                                                         });
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
        const auto &transform = go->GetLocalTransform();
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
    gameObject.SetLocalPosition(position);
}

void NetworkEditorInterface::SetGameObjectRotation(GameObject &gameObject, const vec3 &rotation)
{
    gameObject.SetLocalRotation(DegreesVec3(rotation));
}

void NetworkEditorInterface::IncreseGameObjectRotation(GameObject &gameObject, const vec3 &increseValue)
{
    // vec3 newValue = gameObject.GetTransform().GetRotation().GetEulerDegrees().value + increseValue;
    // gameObject.GetTransform().SetRotation(DegreesVec3(newValue));
}

void NetworkEditorInterface::IncreseGameObjectScale(GameObject &gameObject, const vec3 &value)
{
    vec3 newValue = gameObject.GetWorldTransform().GetScale() + value;

    auto rigidbody = gameObject.GetComponent<Components::Rigidbody>();
    if (rigidbody)
    {
        rigidbody->SetScale(newValue);
    }

    gameObject.SetWorldScale(newValue);
}

vec3 NetworkEditorInterface::GetRotationValueBasedOnKeys(float rotationSpeed, float dir)
{
    vec3 v(0, 0, 0);
    if (scene_.inputManager_->GetKey(editorActions.at(ROTATE_OBJECT_X)))
    {
        v.x = dir * rotationSpeed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(ROTATE_OBJECT_Y)))
    {
        v.y = dir * rotationSpeed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(ROTATE_OBJECT_Z)))
    {
        v.z = dir * rotationSpeed;
    }
    return v;
}

vec3 NetworkEditorInterface::GetPositionChangeValueBasedOnKeys(float speed, float dir)
{
    vec3 v(0, 0, 0);
    if (scene_.inputManager_->GetKey(editorActions.at(MOVE_OBJECT_X)))
    {
        v.x = dir * speed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(MOVE_OBJECT_Y)))
    {
        v.y = dir * speed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(MOVE_OBJECT_Z)))
    {
        v.z = dir * speed;
    }
    return v;
}

vec3 NetworkEditorInterface::GetScaleChangeValueBasedOnKeys(float dir, float speed)
{
    vec3 v(0, 0, 0);
    if (scene_.inputManager_->GetKey(editorActions.at(SCALE_OBJECT_X)))
    {
        v.x = dir * speed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(SCALE_OBJECT_Y)))
    {
        v.y = dir * speed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(SCALE_OBJECT_Z)))
    {
        v.z = dir * speed;
    }
    if (scene_.inputManager_->GetKey(editorActions.at(SCALE_OBJECT)))
    {
        return vec3(dir * speed);
    }
    return v;
}

void NetworkEditorInterface::NewUser(const std::string &str, uint32 id)
{
    LOG_DEBUG << "New user : {" << str << ", " << id << "}";
    userId_ = id;

    if (not scene_.GetFile().empty())
    {
        DebugNetworkInterface::SceneFileMsg msg(scene_.GetFile().GetAbsolutePath().string());
        gateway_.Send(userId_, msg);
    }
}
void NetworkEditorInterface::DisconnectUser(uint32 id)
{
    userId_ = 0;
    LOG_DEBUG << "Disconnect user : {" << id << "}";
    UnsubscribeTransformUpdateIfExist();
    StartScene();
}
void NetworkEditorInterface::OnMessage(Network::UserId, std::unique_ptr<Network::IMessage> msg)
{
    auto textMsg = Network::castMessageAs<Network::TextMessage>(msg.get())->GetText();
    LOG_DEBUG << "Recevied command : " << textMsg;

    auto splitCommand = Utils::SplitString(textMsg, ' ');

    if (splitCommand.empty())
    {
        LOG_DEBUG << "splitCommand empty";
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
        LOG_ERROR << "Unknown command : \"" << command << "\"";
    }
}
void NetworkEditorInterface::LoadSceneFromFile(const EntryParameters &args)
{
    if (args.count("filename") == 0)
    {
        LOG_ERROR << "Filename not found.";
        return;
    }

    File file(args.at("filename"));
    const auto name    = file.GetBaseName();
    auto &sceneManager = scene_.getEngineContext()->GetSceneManager();
    scene_.getEngineContext()->GetSceneManager().SetOnSceneLoadDone([this]() { SetupCamera(); });
    sceneManager.SetActiveScene(name);
}

void NetworkEditorInterface::SaveSceneToFile(const NetworkEditorInterface::EntryParameters &args)
{
    if (args.count("filename") == 0)
    {
        LOG_DEBUG << "Filename not found.";
        return;
    }

    GameEngine::saveSceneToFile(scene_, File(args.at("filename")));
}

void NetworkEditorInterface::QuickSave()
{
    GameEngine::saveSceneToFile(scene_);
}

void NetworkEditorInterface::GetCamera(const EntryParameters &)
{
    UnsubscribeTransformUpdateIfExist();

    DebugNetworkInterface::CameraMsg msg;
    msg.position = scene_.GetCamera().GetPosition();
    msg.rotation = scene_.GetCamera().GetRotation().GetEulerDegrees().value;
    gateway_.Send(userId_, msg);

    cameraChangeSubscriptionId_ = scene_.camera.SubscribeOnChange([&](const auto &) { cameraChangedToSend_.store(true); });
}

void SendChildrenObjectList(uint32 userId, Network::Gateway &gateway, uint32 parentId,
                            const std::vector<std::unique_ptr<GameObject>> &objectList)
{
    if (objectList.empty())
    {
        LOG_ERROR << "No children found. Parent id : " << parentId;
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

    // auto &transform                = gameObject->GetTransform();
    // transformChangeSubscription_   = &transform;
    // auto gameObjectId              = gameObject->GetId();
    // transformChangeSubscriptionId_ = transform.SubscribeOnChange(
    //     [gameObjectId](const auto &)
    //     {
    //         if (not transformChangedToSend_)
    //         {
    //             std::lock_guard<std::mutex> lk(transformChangedMutex_);
    //             transformChangedToSend_ = gameObjectId;
    //         }
    //     });

    // DebugNetworkInterface::Transform msg(gameObject->GetId(), transform.GetPosition(),
    //                                      transform.GetRotation().GetEulerDegrees().value, transform.GetScale());
    // gateway_.Send(userId_, msg);
}

void NetworkEditorInterface::GetGameObjectComponentsListReq(const EntryParameters &param)
{
    if (not param.count("id"))
        return;

    auto gameObject = GetGameObject(param.at("id"));

    if (not gameObject)
        return;

    for (auto &[_, vectorOfComponents] : gameObject->GetComponents())
    {
        for (auto &component : vectorOfComponents)
        {
            DebugNetworkInterface::NewComponentMsgInd componentNameMsg(component->GetTypeName(), component->IsActive());
            gateway_.Send(userId_, componentNameMsg);
        }
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
            LOG_ERROR << "Set position error";
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
            LOG_ERROR << "Set rotation error";
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
                gameObject->SetLocalScale(scale);
            }
            catch (...)
            {
                LOG_ERROR << "Set scale error. Can not convert str to float.";
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
            DeleteGameObject(*go);
        }
        else
        {
            LOG_ERROR << "GameObject not found";
        }
    }
    else
    {
        LOG_ERROR << "Incomplete request";
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
            LOG_ERROR << "GameObject not found";
        }
    }
    else
    {
        LOG_ERROR << "Incomplete request";
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
        LOG_ERROR << "Position parsing stof error";
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
        LOG_ERROR << "Stof error";
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
            gameObject->AddComponent<Components::RendererComponent>().AddModel(GetRelativeDataPath(params.at("filename")));
            gameObject->SetLocalPositionRotation(position, DegreesVec3(rotationEulerDegrees));
            gameObject->AddComponent<Components::MeshShape>();
            gameObject->AddComponent<Components::Rigidbody>().SetMass(0);

            DebugNetworkInterface::NewGameObjectInd message(gameObject->GetId(), 0, gameObject->GetName());
            SetSelectedGameObject(gameObject.get());
            auto parentId = AddGameObject(params, gameObject);
            if (parentId)
            {
                message.parentId = *parentId;
            }
            gateway_.Send(userId_, message);
        }
        catch (...)
        {
            LOG_ERROR << "Exception caught";
        }
    }
    else
    {
        LOG_ERROR << "mandatory param filename not found.";
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

        auto gameObject = GameEngine::SceneReader(scene_).loadPrefab(GetRelativeDataPath(params.at("filename")), goName);

        if (gameObject)
        {
            auto position = scene_.camera.GetPosition();
            position += scene_.camera.GetDirection() * 5.f;
            gameObject->SetLocalPosition(position);

            DebugNetworkInterface::NewGameObjectInd message(gameObject->GetId(), 0, gameObject->GetName());
            gateway_.Send(userId_, message);
        }
    }
}

void NetworkEditorInterface::GetComponentsList(const EntryParameters &)
{
    for (auto [name, _] : Components::ReadFunctions().instance().getComponentTypeNameToId())
    {
        DebugNetworkInterface::AvailableComponentMsgInd msg(name);
        gateway_.Send(userId_, msg);
    }
}

void NetworkEditorInterface::AddComponent(const EntryParameters &params)
{
    if (params.count("id") > 0 and params.count("name") > 0)
    {
        auto &idParam       = params.at("id");
        auto &componentName = params.at("name");

        auto go = GetGameObject(idParam);

        if (not go)
            return;

        if (Components::ReadFunctions().instance().isRegistered(componentName))
        {
            TreeNode node("component");
            node.attributes_.insert({Components::CSTR_TYPE, componentName});
            auto component = go->AddComponent(node);
            if (component)
            {
                component->ReqisterFunctions();

                {
                    auto maybeId = component->getRegisteredFunctionId(Components::FunctionType::Awake);
                    if (maybeId)
                    {
                        scene_.componentController_.callComponentFunction(go->GetId(), Components::FunctionType::Awake, *maybeId);
                    }
                }
                {
                    auto maybeId = component->getRegisteredFunctionId(Components::FunctionType::OnStart);
                    if (maybeId)
                    {
                        scene_.componentController_.callComponentFunction(go->GetId(), Components::FunctionType::OnStart,
                                                                          *maybeId);
                    }
                }

                DebugNetworkInterface::NewComponentMsgInd componentNameMsg(componentName, component->IsActive());
                gateway_.Send(userId_, componentNameMsg);
            }
            else
            {
                LOG_ERROR << "Component : \"" << componentName << "\" creation error.";
            }
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

    Components::IComponent *component{nullptr};
    for (const auto &[_, vectorOfComponents] : gameObject->GetComponents())
    {
        for (auto &c : vectorOfComponents)
        {
            if (c->GetTypeName() == params.at("name"))
            {
                component = c.get();
                break;
            }
        }
    }

    if (not component)
        return;

    std::vector<DebugNetworkInterface::Param> componentParams;
    std::string value{"notSerialiazedValue"};
    for (auto &field : component->GetFields())
    {
        switch (field.type)
        {
            case GameEngine::Components::FieldType::Int:
                value = std::to_string(*static_cast<int *>(field.ptr));
                break;
            case GameEngine::Components::FieldType::UInt:
                value = std::to_string(*static_cast<uint32 *>(field.ptr));
                break;
            case GameEngine::Components::FieldType::Float:
                value = std::to_string(*static_cast<float *>(field.ptr));
                break;
            case GameEngine::Components::FieldType::String:
                value = *static_cast<std::string *>(field.ptr);
                break;
            case GameEngine::Components::FieldType::Boolean:
                value = Utils::BoolToString(*static_cast<bool *>(field.ptr));
                break;
            case GameEngine::Components::FieldType::Texture:
            case GameEngine::Components::FieldType::File:
                value = static_cast<File *>(field.ptr)->GetAbsolutePath().string();
                break;
            case GameEngine::Components::FieldType::Enum:
                break;
            case GameEngine::Components::FieldType::AnimationClip:
                break;
            case GameEngine::Components::FieldType::Vector2i:
                break;
            case GameEngine::Components::FieldType::Vector2f:
                break;
            case GameEngine::Components::FieldType::Vector3f:
                break;
            case GameEngine::Components::FieldType::Vector4f:
                break;
            case GameEngine::Components::FieldType::ColorRGB:
                break;
            case GameEngine::Components::FieldType::ColorRGBA:
                break;
            case GameEngine::Components::FieldType::VectorOfStrings:
                break;
            case GameEngine::Components::FieldType::VectorOfInt:
                break;
            case GameEngine::Components::FieldType::VectorOfFloat:
                break;
            case GameEngine::Components::FieldType::VectorOfFiles:
                break;
            case GameEngine::Components::FieldType::VectorOfTextures:
                break;
            case GameEngine::Components::FieldType::VectorOfAnimationClips:
                break;
            case GameEngine::Components::FieldType::ConstVectorOfTextures:
                break;
        }
        DebugNetworkInterface::Param param(std::string(field.name), value, std::string(magic_enum::enum_name(field.type)));
        componentParams.push_back(param);
    }

    DebugNetworkInterface::ComponentDataMessage msg(params.at("name"), std::stoi(params.at("gameObjectId")), componentParams);
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

void NetworkEditorInterface::ObjectControlAction(float direction, float rotationSpeed, float moveSpeed, float scaleSpeed)
{
    // UseSelectedGameObject(
    //     [this, direction, rotationSpeed, moveSpeed, scaleSpeed](auto &gameObject)
    //     {
    //         IncreseGameObjectRotation(gameObject, GetRotationValueBasedOnKeys(rotationSpeed, direction));
    //         IncreseGameObjectScale(gameObject, GetScaleChangeValueBasedOnKeys(direction, scaleSpeed));
    //         auto moveVector = GetPositionChangeValueBasedOnKeys(moveSpeed, direction);
    //         moveVector      = gameObject.GetTransform().GetRotation().value_ * moveVector;
    //         moveVector      = moveVector + gameObject.GetTransform().GetPosition();
    //         gameObject.GetTransform().SetPosition(moveVector);
    //     });
}

void NetworkEditorInterface::CreateDragObject(GameObject &gameObject)
{
    std::lock_guard<std::mutex> lk(dragObjectMutex_);
    dragObject_ =
        std::make_unique<DragObject>(*scene_.inputManager_, gameObject, scene_.camera, scene_.renderersManager_->GetProjection());
}

void NetworkEditorInterface::ReleaseDragObject()
{
    std::lock_guard<std::mutex> lk(dragObjectMutex_);
    dragObject_.reset(nullptr);
}

void NetworkEditorInterface::SetSelectedGameObject(GameObject *gameObject)
{
    std::lock_guard<std::mutex> m(selectedGameObjectMutex_);

    if (not gameObject and dragObject_)
    {
        ReleaseDragObject();
    }

    bool sentNotif{true};
    if (selectedGameObject_)
    {
        if (gameObject and selectedGameObject_->GetId() == gameObject->GetId())
        {
            return;
        }
    }
    else
    {
        if (not gameObject)
        {
            sentNotif = false;
        }
    }

    if (gameObject)
    {
        auto rigidBody = gameObject->GetComponent<Components::Rigidbody>();
        if (rigidBody)
        {
            rigidBody->SetAsVisualizatedObject();
        }
        else
        {
            scene_.physicsApi_->disableVisualizationForAllRigidbodys();
        }
    }

    if (sentNotif)
    {
        DebugNetworkInterface::SelectedObjectChanged msg(gameObject ? gameObject->GetId() : 0);
        gateway_.Send(userId_, msg);
    }

    selectedGameObject_ = gameObject;
}

void NetworkEditorInterface::UseSelectedGameObject(std::function<void(GameObject &)> action, std::function<void()> notExistAction)
{
    std::lock_guard<std::mutex> m(selectedGameObjectMutex_);
    if (selectedGameObject_)
    {
        action(*selectedGameObject_);
    }
    else
    {
        if (notExistAction)
        {
            notExistAction();
        }
    }
}

void NetworkEditorInterface::UpdateDragObject()
{
    std::lock_guard<std::mutex> lk(dragObjectMutex_);
    if (dragObject_ and not terrainPainter_)
    {
        dragObject_->Update();
    }
}

void NetworkEditorInterface::PaintTerrain()
{
    std::lock_guard<std::mutex> lk(terrainPainterMutex_);
    if (terrainPainter_ and terrainPainterTimer_.GetTimeMiliSeconds() > (1000 / 30))
    {
        auto pointOnTerrain = terrainPainter_->paint();
        if (pointOnTerrain)
        {
            brushCircleTransform_.SetPosition(*pointOnTerrain);
        }
        auto worldScaleBrushSize = terrainPainter_->getWorldScaleBrushSize();
        brushCircleTransform_.SetScale(vec3(worldScaleBrushSize, 1.f, worldScaleBrushSize));
        terrainPainterTimer_.Reset();
    }
}

void NetworkEditorInterface::UpdateArrowsIndicatorPosition()
{
    UseSelectedGameObject(
        [this](auto &gameObject)
        {
            arrowsIndicatorTransform_.SetPositionAndRotation(gameObject.GetWorldTransform().GetPosition(),
                                                             gameObject.GetWorldTransform().GetRotation());
        });
}

void NetworkEditorInterface::SendObjectCreatedNotf(const GameObject &gameObject)
{
    DebugNetworkInterface::NewGameObjectInd message(gameObject.GetId(), 0, gameObject.GetName());
    if (gameObject.GetParent())
    {
        message.parentId = gameObject.GetParent()->GetId();
    }
    gateway_.Send(userId_, message);

    for (const auto &child : gameObject.GetChildren())
    {
        SendObjectCreatedNotf(*child);
    }
}

void NetworkEditorInterface::DeleteGameObject(IdType id)
{
    auto go = scene_.GetGameObject(id);
    if (go)
    {
        DeleteGameObject(*go);
    }
}

void NetworkEditorInterface::DeleteGameObject(GameObject &go)
{
    UnsubscribeTransformUpdateIfExist();

    auto id = go.GetId();
    SetSelectedGameObject(nullptr);
    scene_.RemoveGameObject(go);

    DebugNetworkInterface::GameObjectDeleted msg(id);
    gateway_.Send(userId_, msg);
}

void NetworkEditorInterface::setParamIfExitst(const EntryParameters &params, const std::string &name, bool &value)
{
    auto iter = params.find(name);
    if (iter != params.end())
    {
        try
        {
            value = Utils::StringToBool(iter->second);
        }
        catch (...)
        {
            LOG_ERROR << "Deserialize error for param : " << name << "=" << iter->second;
        }
    }
}

void NetworkEditorInterface::setParamIfExitst(const EntryParameters &params, const std::string &name, int &value)
{
    auto iter = params.find(name);
    if (iter != params.end())
    {
        try
        {
            value = std::stoi(iter->second);
        }
        catch (...)
        {
            LOG_ERROR << "Deserialize error for param : " << name << "=" << iter->second;
        }
    }
}

void NetworkEditorInterface::setParamIfExitst(const EntryParameters &params, const std::string &name, uint32 &value)
{
    auto iter = params.find(name);
    if (iter != params.end())
    {
        try
        {
            value = std::stoi(iter->second);
        }
        catch (...)
        {
            LOG_ERROR << "Deserialize error for param : " << name << "=" << iter->second;
        }
    }
}

void NetworkEditorInterface::setParamIfExitst(const EntryParameters &params, const std::string &name, float &value)
{
    auto iter = params.find(name);
    if (iter != params.end())
    {
        try
        {
            value = std::stof(iter->second);
        }
        catch (...)
        {
            LOG_ERROR << "Deserialize error for param : " << name + "=" << iter->second;
        }
    }
}

void NetworkEditorInterface::setParamIfExitst(const EntryParameters &params, const std::string &name, double &value)
{
    auto iter = params.find(name);
    if (iter != params.end())
    {
        try
        {
            value = std::stof(iter->second);
        }
        catch (...)
        {
            LOG_ERROR << "Deserialize error for param : " << name << "=" << iter->second;
        }
    }
}

template <typename T>
void NetworkEditorInterface::setParamIfExitst(const EntryParameters &params, const std::string &name,
                                              std::optional<T> &maybeValue)
{
    auto iter = params.find(name);
    if (iter != params.end())
    {
        maybeValue = T{};
        setParamIfExitst(params, name, *maybeValue);
    }
}

void NetworkEditorInterface::SetPhysicsVisualization(const EntryParameters &params)
{
    SetDeubgRendererState(DebugRenderer::RenderState::Physics, params);
}

void NetworkEditorInterface::SetPhysicsVisualizationAllObjcts(const EntryParameters &params)
{
    auto enabledPram = params.find("enabled");

    if (enabledPram != params.end())
    {
        auto isEabled = Utils::StringToBool(enabledPram->second);
        if (isEabled)
        {
            SetDeubgRendererState(DebugRenderer::RenderState::Physics, params);
            scene_.physicsApi_->enableVisualizationForAllRigidbodys();
        }
        else
        {
            UseSelectedGameObject(
                [&](GameObject &go)
                {
                    auto rigidBody = go.GetComponent<Components::Rigidbody>();
                    if (rigidBody)
                    {
                        rigidBody->SetAsVisualizatedObject();
                    }
                    else
                    {
                        scene_.physicsApi_->disableVisualizationForAllRigidbodys();
                    }
                },
                [&]() { scene_.physicsApi_->disableVisualizationForAllRigidbodys(); });
        }
    }
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

    SetSelectedGameObject(gameObject);
}

void NetworkEditorInterface::GoCameraToObject(const NetworkEditorInterface::EntryParameters &paramters)
{
    if (not paramters.count("gameObjectId"))
        return;

    auto gameObject = GetGameObject(paramters.at("gameObjectId"));

    if (not gameObject)
        return;

    scene_.camera.SetPosition(gameObject->GetWorldTransform().GetPosition() +
                              (gameObject->GetWorldTransform().GetScale() + vec3(1.f)));
    scene_.camera.LookAt(gameObject->GetWorldTransform().GetPosition());
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

    SetSelectedGameObject(nullptr);

    //  scene_.renderersManager_->GetDebugRenderer().Disable();
    keysSubscriptionsManager_.Clear();
    SetOrignalCamera();
    scene_.inputManager_->StashPopSubscribers();

    scene_.Start();
    scene_.camera.Unlock();
    gateway_.Send(userId_, DebugNetworkInterface::SceneStartedNotifMsg(scene_.GetName()));
}

void NetworkEditorInterface::StopScene()
{
    if (not scene_.start_.load())
        return;

    scene_.camera.Lock();
    UnsubscribeTransformUpdateIfExist();
    scene_.Stop();

    scene_.inputManager_->StashSubscribers();
    SetupCamera();
    KeysSubscribtions();
    scene_.renderersManager_->GetDebugRenderer().Enable();

    gateway_.Send(userId_, DebugNetworkInterface::SceneStopedNotifMsg(scene_.GetName()));
}

void NetworkEditorInterface::ModifyComponentReq(const EntryParameters &params)
{
    if (not params.count("gameObjectId") or not params.count("name"))
        return;

    auto gameObject = GetGameObject(params.at("gameObjectId"));

    if (not gameObject)
        return;

    Components::IComponent *component{nullptr};
    for (const auto &[_, vectorOfComponents] : gameObject->GetComponents())
    {
        for (auto &c : vectorOfComponents)
        {
            if (c->GetTypeName() == params.at("name"))
            {
                component = c.get();
                break;
            }
        }
    }

    if (not component)
        return;

    auto p    = params;
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

    auto fields = component->GetFields();
    for (const auto &[name, value] : params)
    {
        try
        {
            auto iter = std::find_if(fields.begin(), fields.end(), [n = name](const auto &field) { return n == field.name; });
            if (iter != fields.end())
            {
                auto &field = *iter;
                switch (field.type)
                {
                    case GameEngine::Components::FieldType::Int:
                    {
                        auto &val = *static_cast<int *>(field.ptr);
                        val       = std::stoi(value);
                        break;
                    }
                    case GameEngine::Components::FieldType::UInt:
                    {
                        auto &val = *static_cast<uint32 *>(field.ptr);
                        val       = std::stoi(value);
                    }
                    break;
                    case GameEngine::Components::FieldType::Float:
                    {
                        auto &val = *static_cast<float *>(field.ptr);
                        val       = std::stof(value);
                    }
                    break;
                    case GameEngine::Components::FieldType::String:
                    {
                        auto &val = *static_cast<std::string *>(field.ptr);
                        val       = value;
                    }
                    break;
                    case GameEngine::Components::FieldType::Boolean:
                    {
                        auto &val = *static_cast<bool *>(field.ptr);
                        val       = Utils::StringToBool(value);
                    }
                    break;
                    case GameEngine::Components::FieldType::Texture:
                    case GameEngine::Components::FieldType::File:
                    {
                        auto &val = *static_cast<File *>(field.ptr);
                        val       = value;
                    }
                    break;
                    case GameEngine::Components::FieldType::Enum:
                        break;
                    case GameEngine::Components::FieldType::AnimationClip:
                        break;
                    case GameEngine::Components::FieldType::Vector2i:
                        break;
                    case GameEngine::Components::FieldType::Vector2f:
                        break;
                    case GameEngine::Components::FieldType::Vector3f:
                        break;
                    case GameEngine::Components::FieldType::Vector4f:
                        break;
                    case GameEngine::Components::FieldType::ColorRGB:
                        break;
                    case GameEngine::Components::FieldType::ColorRGBA:
                        break;
                    case GameEngine::Components::FieldType::VectorOfStrings:
                        break;
                    case GameEngine::Components::FieldType::VectorOfInt:
                        break;
                    case GameEngine::Components::FieldType::VectorOfFloat:
                        break;
                    case GameEngine::Components::FieldType::VectorOfFiles:
                        break;
                    case GameEngine::Components::FieldType::VectorOfTextures:
                        break;
                    case GameEngine::Components::FieldType::VectorOfAnimationClips:
                        break;
                    case GameEngine::Components::FieldType::ConstVectorOfTextures:
                        break;
                }
            }
        }
        catch (...)
        {
            LOG_WARN << "set param error " << name;
        }
    }
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
    scene_.getEngineContext()->AddEngineEvent(ChangeSceneEvent{ChangeSceneEvent::Type::RELOAD_SCENE});
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

DebugNetworkInterface::TerrainPainterEnabled PrepareTerrainPainterEnabledMsg(Painter &painter, int32 &brushSize)
{
    DebugNetworkInterface::TerrainPainterEnabled msg;
    msg.type               = std::to_string(painter.getPaintType());
    msg.strength           = painter.strength();
    msg.brushSize          = painter.brushSize();
    msg.selectedBrushType  = painter.selectedBrush();
    msg.stepInterpolation  = std::to_string(painter.stepInterpolation());
    msg.stepInterpolations = AvaiableStepInterpolationsStrs();
    msg.brushTypes         = painter.avaiableBrushTypes();
    brushSize              = painter.brushSize();
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
    gateway_.Send(userId_, PrepareTerrainPainterEnabledMsg(*terrainPainter_, brushSize_));
}

void NetworkEditorInterface::EnableTerrainTexturePainter(const EntryParameters &)
{
    DisableTerrainPainter({});
    terrainPainter_ = std::make_unique<TerrainTexturePainter>(GetPainterEntryParameters(), Color(255, 0, 0));
    gateway_.Send(userId_, PrepareTerrainPainterEnabledMsg(*terrainPainter_, brushSize_));
}

void NetworkEditorInterface::EnablePlantPainter(const EntryParameters &params)
{
    if (not params.count("gameObjectId"))
    {
        LOG_ERROR << "GameObject Id is necessary to get plant component.";
        return;
    }

    auto gameObject = GetGameObject(params.at("gameObjectId"));

    if (gameObject)
    {
        auto component = gameObject->GetComponent<Components::GrassRendererComponent>();

        if (not component)
        {
            LOG_ERROR << "Grass component not found in object : " << params.at("gameObjectId");
            return;
        }

        DisableTerrainPainter({});
        terrainPainter_ = std::make_unique<PlantPainter>(GetPainterEntryParameters(), *component);
        gateway_.Send(userId_, PrepareTerrainPainterEnabledMsg(*terrainPainter_, brushSize_));
    }
}

void NetworkEditorInterface::DisableTerrainPainter(const EntryParameters &)
{
    std::lock_guard<std::mutex> lk(terrainPainterMutex_);
    if (terrainPainter_)
    {
        terrainPainter_.reset(nullptr);
    }
    brushCircleTransform_.SetPosition(vec3(std::numeric_limits<float>::max()));
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
            terrainPainter_->strength(std::stof(params.at("strength")));
        }
        if (params.count("brushSize"))
        {
            terrainPainter_->brushSize(std::stoi(params.at("brushSize")));
            brushSize_ = std::stoi(params.at("brushSize"));
        }
        if (params.count("stepInterpolation"))
        {
            StepInterpolation step;
            std::from_string(params.at("stepInterpolation"), step);
            terrainPainter_->stepInterpolation(step);
        }
        if (params.count("brushType"))
        {
            terrainPainter_->setBrush(params.at("brushType"));
        }
        if (params.count("color"))
        {
            if (terrainPainter_->getPaintType() != PaintType::BlendMap)
            {
                LOG_ERROR << "Incompatible paint mode.";
                return;
            }

            auto inputColor = params.at("color");
            int i           = 0;
            Color color;
            for (auto c : inputColor)
            {
                if (i < 4)
                    color[i++] = c == '1' ? 1.f : 0.f;
                else
                    LOG_ERROR << "to many bits.";
            }
            static_cast<TerrainTexturePainter *>(terrainPainter_.get())->setColor(color);
        }
        if (params.count("generate") and terrainPainter_->getPaintType() == PaintType::Plant)
        {
            static_cast<PlantPainter *>(terrainPainter_.get())->generatePositions();
        }
        if (params.count("eraseMode") and terrainPainter_->getPaintType() == PaintType::Plant)
        {
            static_cast<PlantPainter *>(terrainPainter_.get())->eraseMode();
        }
    }
    catch (...)
    {
        LOG_ERROR << "Message parsing error.";
    }
}

void NetworkEditorInterface::RecalculateTerrainNormals(const NetworkEditorInterface::EntryParameters &)
{
    std::lock_guard<std::mutex> lk(terrainPainterMutex_);
    if (not terrainPainter_ or terrainPainter_->getPaintType() == PaintType::HeightMap)
        return;

    static_cast<TerrainHeightPainter *>(terrainPainter_.get())->recalculateTerrainNormals();
}
void NetworkEditorInterface::ClearTerrainsBlendMap(const EntryParameters &)
{
    auto terrains = scene_.componentController_.GetAllComponentsOfType<Components::TerrainRendererComponent>();

    for (auto &terrain : terrains)
    {
        auto image    = CreateZerosImage<uint8>(vec2ui(4096, 4096), 4);
        auto blendMap = static_cast<GeneralTexture *>(terrain->GetTexture(TerrainTextureType::blendMap));
        blendMap->SetImage(std::move(image));
        terrain->BlendMapChanged();
    }
}

void NetworkEditorInterface::GenerateTerrains(const EntryParameters &params)
{
    TerrainHeightGenerator::EntryParamters entryParamters;
    bool updateNoiseSeed{true};

    setParamIfExitst(params, "gameObjectId", entryParamters.gameObjectId);
    setParamIfExitst(params, "bias", entryParamters.bias);
    setParamIfExitst(params, "octaves", entryParamters.octaves);
    setParamIfExitst(params, "width", entryParamters.perTerrainHeightMapsize.x);
    setParamIfExitst(params, "height", entryParamters.perTerrainHeightMapsize.y);
    setParamIfExitst(params, "updateNoiseSeed", updateNoiseSeed);

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

    scene_.resourceManager_->GetGpuResourceLoader().AddFunctionToCall([&]() { scene_.renderersManager_->UpdatePerAppBuffer(); });
}

void NetworkEditorInterface::CreateTerrain(const NetworkEditorInterface::EntryParameters &params)
{
    std::string heightMapFile{};
    if (params.count("filename"))
    {
        heightMapFile = std::filesystem::path(params.at("filename")).filename().stem().string();
    }
    else
    {
        std::string heightMapName{"heightMap"};
        if (params.count("heightMapName"))
        {
            heightMapName = params.at("heightMapName");
        }
        vec2ui size(513);
        if (params.count("sizeX"))
        {
            size.x = std::stoi(params.at("sizeX"));
        }
        if (params.count("sizeY"))
        {
            size.y = std::stoi(params.at("sizeY"));
        }

        heightMapFile = heightMapName + "_" + std::to_string(size.x) + "_" + std::to_string(size.y) + ".terrain";
        CreateHeightMap(heightMapFile, size);
    }

    std::string goName{"Terrain"};
    if (params.count("name"))
    {
        goName = params.at("name");
    }

    auto gameObject = scene_.CreateGameObject(goName);
    try
    {
        auto &terrainComponent = gameObject->AddComponent<Components::TerrainRendererComponent>();

        std::vector<Components::TerrainComponentBase::TerrainTexture> textures{
            {heightMapFile, 1.f, TerrainTextureType::heightmap}};
        terrainComponent.LoadTextures(textures);

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
        LOG_ERROR << "Exception caught";
    }
}

void NetworkEditorInterface::GenerateTerrainBlendMapToFile()
{
    auto terrains = scene_.componentController_.GetAllComponentsOfType<Components::TerrainRendererComponent>();

    std::set<std::string> generetedBlendMaps;

    for (auto &terrain : terrains)
    {
        const auto &heightMap = *terrain->GetHeightMap();

        auto heightMapFile = *terrain->GetTexture(TerrainTextureType::heightmap)->GetFile();

        if (not generetedBlendMaps.count(heightMapFile.GetAbsolutePath().string()))
        {
            heightMapFile.AddSuffixToBaseName("_generatedBlendmap");

            GenerateBlendMap(terrain->GetTerrainConfiguration().GetScale(), heightMap, heightMapFile);
            generetedBlendMaps.insert(heightMapFile.GetAbsolutePath().string());
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
    scene_.engineContext->AddEngineEvent(EngineEvent(QuitEvent::QUIT));
}

void NetworkEditorInterface::MoveObjectToCameraPosition(const EntryParameters &params)
{
    auto iter = params.find("gameObjectId");

    if (iter != params.end())
    {
        auto gameObject = GetGameObject(iter->second);

        if (gameObject)
        {
            auto newPostion = scene_.GetCamera().GetPosition() + (scene_.GetCamera().GetDirection() * 5.f);
            gameObject->SetWorldPosition(newPostion);
        }
    }
}

void NetworkEditorInterface::ModelPreviewRequest(const EntryParameters &params)
{
    auto input  = params.find("input");
    auto output = params.find("output");

    if (input != params.end() and output != params.end())
    {
        auto gameObject = scene_.CreateGameObject();
        auto &component = gameObject->AddComponent<Components::PreviewComponent>();
        component.addModel(input->second, output->second);
        scene_.AddGameObject(std::move(gameObject));
    }
}

void NetworkEditorInterface::ChangeGameObjectParent(const EntryParameters &params)
{
    auto gameObjectIdIter          = params.find("gameObjectId");
    auto newParentGameObjectIdIter = params.find("newParentGameObjectId");

    if (gameObjectIdIter != params.end() and newParentGameObjectIdIter != params.end())
    {
        try
        {
            auto gameObjectId = std::stoi(gameObjectIdIter->second);
            auto newParentId  = std::stoi(newParentGameObjectIdIter->second);

            scene_.ChangeParent(gameObjectId, newParentId);
        }
        catch (...)
        {
            LOG_ERROR << "Invalid convert gameObject str to int.";
        }
    }
}

void NetworkEditorInterface::CloneGameObject(const EntryParameters &params)
{
    auto gameObjectIdIter = params.find("gameObjectId");
    if (gameObjectIdIter != params.end())
    {
        auto gameObject = GetGameObject(gameObjectIdIter->second);
        if (gameObject)
        {
            auto clonedGameObject = GameEngine::cloneGameObject(scene_, *gameObject);
            if (clonedGameObject)
            {
                SetSelectedGameObject(clonedGameObject);
                SendObjectCreatedNotf(*clonedGameObject);
            }
        }
    }
}

void NetworkEditorInterface::CloneGameObjectInstancesWithRandomPosition(const EntryParameters &params)
{
    auto gameObjectIdIter = params.find("gameObjectId");
    auto instancesIter    = params.find("instances");
    auto minRangeXIter    = params.find("minRangeX");
    auto minRangeZIter    = params.find("minRangeZ");
    auto maxRangeXIter    = params.find("maxRangeX");
    auto maxRangeZIter    = params.find("maxRangeZ");

    if (gameObjectIdIter != params.end() and instancesIter != params.end() and minRangeXIter != params.end() and
        minRangeZIter != params.end() and maxRangeXIter != params.end() and maxRangeZIter != params.end())
    {
        auto gameObject = GetGameObject(gameObjectIdIter->second);
        if (gameObject)
        {
            try
            {
                auto instances = std::stoi(instancesIter->second);
                auto minX      = std::stof(minRangeXIter->second);
                auto minZ      = std::stof(minRangeZIter->second);
                auto maxX      = std::stof(maxRangeXIter->second);
                auto maxZ      = std::stof(maxRangeZIter->second);

                auto container = scene_.CreateGameObject("instances_clone_" + gameObject->GetName());
                SendObjectCreatedNotf(*container);

                auto containerPtr = container.get();
                scene_.AddGameObject(std::move(container));

                for (int i = 0; i < instances; ++i)
                {
                    auto clonedGameObject = GameEngine::cloneGameObject(scene_, *gameObject);
                    if (clonedGameObject)
                    {
                        clonedGameObject->SetName(gameObject->GetName() + "_instance_" + std::to_string(i));
                        scene_.ChangeParent(*clonedGameObject, *containerPtr);

                        auto x = getRandomFloat(minX, maxX);
                        auto z = getRandomFloat(minZ, maxZ);
                        // auto roatateY = getRandomFloat(0.f, 360.f);
                        auto scale = getRandomFloat(0.8f, 1.2f);

                        auto goScale    = gameObject->GetWorldTransform().GetScale() * scale;
                        auto goRotation = gameObject->GetWorldTransform()
                                              .GetRotation()
                                              .value_;  // *Rotation(DegreesVec3(0, roatateY, 0)).value_;

                        auto hit = scene_.getHeightPositionInWorld(x, z);

                        clonedGameObject->SetWorldPositionRotationScale(
                            hit ? (hit->pointWorld - vec3(0, 0.05f * goScale.y, 0)) : vec3(x, 0.f, z), goRotation, goScale);

                        SendObjectCreatedNotf(*clonedGameObject);
                    }
                }
            }
            catch (...)
            {
                LOG_ERROR << "Something went wrong";
            }
        }
    }
}

void NetworkEditorInterface::CreatePrefabFromObject(const EntryParameters &params)
{
    auto gameObjectIdIter = params.find("gameObjectId");
    auto filenameIter     = params.find("filename");

    if (gameObjectIdIter != params.end() and filenameIter != params.end())
    {
        auto gameObject = GetGameObject(gameObjectIdIter->second);
        if (gameObject)
        {
            GameEngine::createAndSavePrefab(GameEngine::File{filenameIter->second}, *gameObject);
        }
    }
}

void NetworkEditorInterface::CreateTerrainTranstion(const NetworkEditorInterface::EntryParameters &params)
{
    auto paramTerrainGameObjectId1 = params.find("terrainGameObjectId1");
    auto paramTerrainGameObjectId2 = params.find("terrainGameObjectId2");

    float transition = 5.f;
    setParamIfExitst(params, "transitionSize", transition);

    if (paramTerrainGameObjectId1 != params.end() and paramTerrainGameObjectId2 != params.end())
    {
        auto go1 = GetGameObject(paramTerrainGameObjectId1->second);
        auto go2 = GetGameObject(paramTerrainGameObjectId2->second);

        if (go1 and go2)
            createTerrainTransition(*go1, *go2, transition);
    }
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
        LOG_ERROR << "Invalid convert gameObject str to int.";
    }

    LOG_ERROR << "GameObject not found. " << gameObjectId;
    return nullptr;
}
void NetworkEditorInterface::UnsubscribeTransformUpdateIfExist()
{
    if (transformChangeSubscriptionId_)
    {
        if (not transformChangeSubscription_)
        {
            LOG_ERROR << "Somthing went wrong. transformChangeSubscription_ is nullptr";
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
        cameraChangeSubscriptionId_ = std::nullopt;
    }
}

void NetworkEditorInterface::SetOrignalCamera()
{
    if (cameraEditorId_)
    {
        scene_.camera.remove(*cameraEditorId_);
        scene_.camera.Unlock();
        cameraEditorId_ = std::nullopt;
    }
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
            result                = parentGameObject->GetId();
            auto go               = gameObject.get();
            const auto &transform = gameObject->GetWorldTransform();
            auto worldPosition    = transform.GetPosition();
            auto worldRotation    = transform.GetRotation();
            auto worldScale       = transform.GetScale();

            scene_.AddGameObject(*parentGameObject, std::move(gameObject));
            go->SetWorldPositionRotationScale(worldPosition, worldRotation, worldScale);
        }
        else
        {
            LOG_ERROR << "Parent not found, parentId=" << params.at("parentGameObjectId");
        }
    }
    else
    {
        scene_.AddGameObject(std::move(gameObject));
    }
    return result;
}
}  // namespace GameEngine
