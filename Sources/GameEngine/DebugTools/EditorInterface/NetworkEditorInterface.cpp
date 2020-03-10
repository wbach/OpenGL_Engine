#include "NetworkEditorInterface.h"

#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include <algorithm>

#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/AvailableComponentMsgInd.h"
#include "Messages/CameraMsg.h"
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
}
NetworkEditorInterface::NetworkEditorInterface(Scene &scene)
    : scene_(scene)
    , isRunning_{true}
    , userId_{0}
{
    firstPersonCamera = std::make_unique<FirstPersonCamera>(scene.inputManager_, scene.displayManager_);
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

    networkThread_ = std::thread([&]() {
        DEBUG_LOG("Starting gateway thread");

        while (isRunning_.load())
        {
            gateway_.Update();
        }
    });
}

NetworkEditorInterface::~NetworkEditorInterface()
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

    if (cameraChangeSubscriptionId_)
    {
        scene_.camera.UnsubscribeOnChange(*cameraChangeSubscriptionId_);
    }

    isRunning_.store(false);

    if (networkThread_.joinable())
    {
        networkThread_.join();
    }
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
        DebugNetworkInterface::Transform msg(gameObjectId, transform.GetPosition(), transform.GetRotation(),
                                             transform.GetScale());
        gateway_.Send(userId_, msg);
    });

    DebugNetworkInterface::Transform msg(gameObject->GetId(), transform.GetPosition(), transform.GetRotation(),
                                         transform.GetScale());
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
                    gameObject->worldTransform.SetRotation(rotation);
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
