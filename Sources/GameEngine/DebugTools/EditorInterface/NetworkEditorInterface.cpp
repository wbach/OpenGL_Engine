#include "NetworkEditorInterface.h"

#include <Input/InputManager.h>
#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include <algorithm>

#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/DebugTools/MousePicker/MousePicker.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/AvailableComponentMsgInd.h"
#include "Messages/CameraMsg.h"
#include "Messages/SelectedObjectChanged.h"
#include "Messages/ComponentDataMessage.h"
#include "Messages/NewComponentMsgInd.h"
#include "Messages/NewGameObjectInd.h"
#include "Messages/RemoveComponentMsgInd.h"
#include "Messages/RemoveGameObjectInd.h"
#include "Messages/Transform.h"
#include "Messages/XmlMessageConverter.h"

namespace GameEngine
{
namespace
{
std::unique_ptr<FirstPersonCamera> firstPersonCamera;
}  // namespace

class DragObject
{
public:
    DragObject(Input::InputManager &manager, common::Transform& positionIndicator, common::Transform &transform, const CameraWrapper &camera,
               const Projection &projection)
        : input_(manager)
        , positionIndicator_(positionIndicator)
        , transform_(transform)
        , camera_(camera)
        , projection_(projection)
    {
        mouseZcoord_ = CalculateMouseZCoord(transform_.GetPosition());
        DEBUG_LOG(std::to_string(mouseZcoord_));
        offset_ = transform_.GetPosition() - GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
    }

    void Update()
    {
        auto mouseWorldPoint = GetMouseAsWorldPoint(input_.GetMousePosition(), mouseZcoord_);
        transform_.SetPosition(mouseWorldPoint + offset_);
        positionIndicator_.SetPosition(mouseWorldPoint + offset_);
    }

private:
    vec3 WorldToScreenPoint(const vec3 &point)
    {
        return Utils::Vec4ToVec3(projection_.GetProjectionMatrix() * camera_.GetViewMatrix() * vec4(point, 1.f));
    }

    vec3 ScreenToWorldPoint(const vec3 &point)
    {
        auto eyeCoords   = glm::inverse(projection_.GetProjectionMatrix()) * vec4(point, 1.f);
        auto worldCoords = glm::inverse(camera_.GetViewMatrix()) * eyeCoords;
        return Utils::Vec4ToVec3(worldCoords);
    }

    float CalculateMouseZCoord(const vec3 &objectPosition)
    {
        return WorldToScreenPoint(objectPosition).z;
    }

    vec3 GetMouseAsWorldPoint(const vec2 &mousePosition, float zCoord)
    {
        return ScreenToWorldPoint(vec3(mousePosition, zCoord));
    }

private:
    Input::InputManager &input_;
    common::Transform& positionIndicator_;
    common::Transform &transform_;
    const CameraWrapper &camera_;
    const Projection &projection_;
    vec3 offset_;
    float mouseZcoord_;
};

NetworkEditorInterface::NetworkEditorInterface(Scene &scene)
    : scene_(scene)
    , transformChangeSubscription_{nullptr}
    , selectedGameObject_{nullptr}
    , userId_{0}
{
    firstPersonCamera = std::make_unique<FirstPersonCamera>(*scene.inputManager_, *scene.displayManager_);
    firstPersonCamera->Lock();

    cameraLockUnlockKeySubscribtion_ = scene.inputManager_->SubscribeOnKeyDown(KeyCodes::LCTRL, [&]() {
        if (firstPersonCamera->IsLocked())
        {
            firstPersonCamera->Unlock();
        }
        else
        {
            firstPersonCamera->Lock();
        }
    });
    SetFreeCamera();

    // scene_.Stop();
    commands_.insert({"openFile", [&](const EntryParameters &v) { LoadSceneFromFile(v); }});
    commands_.insert({"getObjectList", [&](const EntryParameters &v) { GetObjectList(v); }});
    commands_.insert({"transformReq", [&](const EntryParameters &v) { TransformReq(v); }});
    commands_.insert(
        {"getGameObjectComponentsListReq", [&](const EntryParameters &v) { GetGameObjectComponentsListReq(v); }});
    commands_.insert({"setPosition", [&](const EntryParameters &v) { SetGameObjectPosition(v); }});
    commands_.insert({"setRotation", [&](const EntryParameters &v) { SetGameObjectRotation(v); }});
    commands_.insert({"setScale", [&](const EntryParameters &v) { SetGameObjectScale(v); }});
    commands_.insert({"createGameObject", [&](const EntryParameters &v) { CreateGameObject(v); }});
    commands_.insert({"addComponent", [&](const EntryParameters &v) { AddComponent(v); }});
    commands_.insert({"startScene", [&](const EntryParameters &v) { StartScene(v); }});
    commands_.insert({"stopScene", [&](const EntryParameters &v) { StopScene(v); }});
    commands_.insert({"getComponentList", [&](const EntryParameters &v) { GetComponentsList(v); }});
    commands_.insert({"getComponentParams", [&](const EntryParameters &v) { GetComponentParams(v); }});
    commands_.insert({"getCamera", [&](const EntryParameters &v) { GetCamera(v); }});
    commands_.insert({"modifyComponentReq", [&](const EntryParameters &v) { ModifyComponentReq(v); }});

    gateway_.AddMessageConverter(std::make_unique<DebugNetworkInterface::XmlMessageConverter>());

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
             if (dragObject_)
              dragObject_->Update();
        },
        "NetworkEditorFps");

    keyDownSub_ = scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        MousePicker mousePicker(scene_.camera, scene_.renderersManager_->GetProjection(), EngineConf.window.size);

        selectedGameObject_ =
            mousePicker.SelectObject(scene_.inputManager_->GetMousePosition(), scene_.GetGameObjects());

        if (selectedGameObject_)
        {
            DEBUG_LOG("selected object : " + selectedGameObject_->GetName());
            arrowsIndicatorTransform_.SetPositionAndRotation(selectedGameObject_->worldTransform.GetPosition(),
                                                             selectedGameObject_->worldTransform.GetRotation());
            dragObject_ = std::make_unique<DragObject>(*scene_.inputManager_, arrowsIndicatorTransform_, selectedGameObject_->worldTransform,
                                                       scene_.camera, scene_.renderersManager_->GetProjection());

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

    keyUpSub_ = scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() {
        selectedGameObject_ = nullptr;
        dragObject_         = nullptr;
    });

    auto model = scene_.resourceManager_->LoadModel("Meshes/Indicators/Arrows.obj");

    if (model)
    {
        scene_.renderersManager_->GetDebugRenderer().AddDebugObject(*model, arrowsIndicatorTransform_);
    }
}

NetworkEditorInterface::~NetworkEditorInterface()
{
    scene_.inputManager_->UnsubscribeOnKeyDown(KeyCodes::LMOUSE, keyDownSub_);
    scene_.inputManager_->UnsubscribeOnKeyUp(KeyCodes::LMOUSE, keyUpSub_);
    scene_.inputManager_->UnsubscribeOnKeyDown(KeyCodes::LCTRL, cameraLockUnlockKeySubscribtion_);

    if (transformChangeSubscriptionId_)
    {
        if (not transformChangeSubscription_)
        {
            ERROR_LOG("Somthing went wrong. transformChangeSubscription_ is nullptr");
            return;
        }

        transformChangeSubscription_->UnsubscribeOnChange(*transformChangeSubscriptionId_);
    }

    if (cameraChangeSubscriptionId_)
    {
        scene_.camera.UnsubscribeOnChange(*cameraChangeSubscriptionId_);
    }

    EngineContext.threadSync_.Unsubscribe(threadId_);
}
void NetworkEditorInterface::AddObject(const std::string &path)
{
    DEBUG_LOG("AddObject not implemented : path=" + path);
}

void NetworkEditorInterface::NewUser(const std::string &str, uint32 id)
{
    DEBUG_LOG("New user : {" + str + ", " + std::to_string(id) + "}");
    userId_ = id;
}
void NetworkEditorInterface::DisconnectUser(uint32 id)
{
    userId_ = 0;
    DEBUG_LOG("Disconnect user : {" + std::to_string(id) + "}");
    UnsubscribeTransformUpdateIfExist();
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

void NetworkEditorInterface::GetCamera(const EntryParameters &)
{
    DebugNetworkInterface::CameraMsg msg;
    msg.position = scene_.GetCamera().GetPosition();
    msg.rotation = scene_.GetCamera().GetRotation();
    gateway_.Send(userId_, msg);

    cameraChangeSubscriptionId_ = scene_.camera.SubscribeOnChange([&](const auto &camera) {
        DebugNetworkInterface::CameraMsg msg;
        msg.position = camera.GetPosition();
        msg.rotation = camera.GetRotation();
        gateway_.Send(userId_, msg);
    });
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

    auto &transform                = gameObject->worldTransform;
    transformChangeSubscription_   = &transform;
    auto gameObjectId              = gameObject->GetId();
    transformChangeSubscriptionId_ = transform.SubscribeOnChange([this, gameObjectId](const auto &transform) {
        DebugNetworkInterface::Transform msg(gameObjectId, transform.GetPosition(),
                                             transform.GetRotation().GetEulerDegrees().value, transform.GetScale());
        gateway_.Send(userId_, msg);
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
                auto rigidbody = gameObject->GetComponent<Components::Rigidbody>();
                if (rigidbody)
                {
                    rigidbody->SetPosition(position);
                }
                else
                {
                    gameObject->worldTransform.SetPosition(position);
                    gameObject->worldTransform.TakeSnapShoot();
                }
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
                auto rigidbody = gameObject->GetComponent<Components::Rigidbody>();
                if (rigidbody)
                {
                    rigidbody->SetRotation(rotation);
                }
                else
                {
                    gameObject->worldTransform.SetRotation(DegreesVec3(rotation));
                    gameObject->worldTransform.TakeSnapShoot();
                }
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
    if (params.empty())
    {
        DEBUG_LOG("param is empty");
        return;
    }

    std::string goName("NewGameObject");
    if (params.count("name") > 1)
    {
        goName = params.at("name");
    }

    auto go = scene_.CreateGameObject(goName);
    scene_.AddGameObject(go);
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

void NetworkEditorInterface::StartScene(const EntryParameters &)
{
    scene_.Start();
}

void NetworkEditorInterface::StopScene(const EntryParameters &)
{
    scene_.Stop();
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

void NetworkEditorInterface::SetFreeCamera()
{
    auto cameraPosition = scene_.camera.GetPosition();
    auto cameraRotation = scene_.camera.GetRotation();
    firstPersonCamera->SetPosition(cameraPosition);
    firstPersonCamera->SetRotation(cameraRotation);
    scene_.SetCamera(*firstPersonCamera);
}
}  // namespace GameEngine
