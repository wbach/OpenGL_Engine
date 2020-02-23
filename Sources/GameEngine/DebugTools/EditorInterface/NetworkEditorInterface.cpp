#include "NetworkEditorInterface.h"

#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/AvailableComponentMsgInd.h"
#include "Messages/CameraMsg.h"
#include "Messages/NewComponentMsgInd.h"
#include "Messages/NewGameObjectIndMsg.h"
#include "Messages/RemoveComponentMsgInd.h"
#include "Messages/RemoveGameObjectIndMsg.h"
#include "Messages/TransformMsg.h"
#include "XMLMessageConverter.h"

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

    scene_.Stop();
    commands_.insert({"openFile", [&](const std::vector<std::string> &v) { LoadSceneFromFile(v); }});
    commands_.insert({"getObjectList", [&](const std::vector<std::string> &v) { GetObjectList(v); }});
    commands_.insert({"transformReq", [&](const std::vector<std::string> &v) { TransformReq(v); }});
    commands_.insert({"getGameObjectComponentsListReq",
                      [&](const std::vector<std::string> &v) { GetGameObjectComponentsListReq(v); }});
    commands_.insert({"setPosition", [&](const std::vector<std::string> &v) { SetGameObjectPosition(v); }});
    commands_.insert({"setRotation", [&](const std::vector<std::string> &v) { SetGameObjectRotation(v); }});
    commands_.insert({"setScale", [&](const std::vector<std::string> &v) { SetGameObjectScale(v); }});
    commands_.insert({"createGameObject", [&](const std::vector<std::string> &v) { CreateGameObject(v); }});
    commands_.insert({"addComponent", [&](const std::vector<std::string> &v) { AddComponent(v); }});
    commands_.insert({"startScene", [&](const std::vector<std::string> &v) { StartScene(v); }});
    commands_.insert({"stopScene", [&](const std::vector<std::string> &v) { StopScene(v); }});
    commands_.insert({"getComponentList", [&](const std::vector<std::string> &v) { GetComponentsList(v); }});
    commands_.insert({"getCamera", [&](const std::vector<std::string> &v) { GetCamera(v); }});

    gateway_.AddMessageConverter(std::make_unique<GameEngine::XmlMessageConverter>());

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
        commands_.at(splitCommand[0])(splitCommand);
    }
    else
    {
        DEBUG_LOG("Unknown command : \"" + splitCommand[0] + "\"");
    }
}
void NetworkEditorInterface::LoadSceneFromFile(const std::vector<std::string> &args)
{
    if (args.empty())
    {
        DEBUG_LOG("Filename not found.");
        return;
    }

    scene_.LoadFromFile(args[1]);
}

void NetworkEditorInterface::GetCamera(const std::vector<std::string> &)
{
    CameraMsg msg;
    msg.position_ = scene_.GetCamera().GetPosition();
    msg.rotation_ = scene_.GetCamera().GetRotation();
    gateway_.Send(userId_, msg);

    scene_.camera.SubscribeOnChange([&](const auto &camera) {
        CameraMsg msg;
        msg.position_ = camera.GetPosition();
        msg.rotation_ = camera.GetRotation();
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
        NewGameObjectIndMsg message(go->GetName());
        message.id       = go->GetId();
        message.parentId = parentId;
        gateway.Send(userId, message);

        const auto &children = go->GetChildrens();
        SendChildrenObjectList(userId, gateway, go->GetId(), children);
    }
}

void NetworkEditorInterface::GetObjectList(const std::vector<std::string> &)
{
    DEBUG_LOG("");

    auto &objectList = scene_.GetGameObjects();
    if (not objectList.empty())
    {
        for (auto &go : objectList)
        {
            NewGameObjectIndMsg message(go.second->GetName());
            message.id = go.second->GetId();
            gateway_.Send(userId_, message);
            SendChildrenObjectList(userId_, gateway_, go.second->GetId(), go.second->GetChildrens());
        }
    }
    // NewGameObjectIndMsg message("Camera");
    //   message.id = go.second->GetId();
}

void NetworkEditorInterface::TransformReq(const std::vector<std::string> &v)
{
    if (v.empty())
    {
        DEBUG_LOG("object id not found.");
        return;
    }

    auto gameObject = GetGameObjectBasedOnParam(v[1]);

    if (not gameObject)
    {
        DEBUG_LOG("GameObject not found. " + v[1]);
        return;
    }

    UnsubscribeTransformUpdateIfExist();

    auto &transform                = gameObject->worldTransform;
    transformChangeSubscription_   = &transform;
    auto gameObjectId              = gameObject->GetId();
    transformChangeSubscriptionId_ = transform.SubscribeOnChange([this, gameObjectId](const auto &transform) {
        TransformMsg msg(gameObjectId, transform.GetPosition(), transform.GetRotation(), transform.GetScale());
        gateway_.Send(userId_, msg);
    });

    TransformMsg msg(gameObject->GetId(), transform.GetPosition(), transform.GetRotation(), transform.GetScale());
    gateway_.Send(userId_, msg);
}

void NetworkEditorInterface::GetGameObjectComponentsListReq(const std::vector<std::string> &v)
{
    if (v.empty())
    {
        DEBUG_LOG("object id not found.");
        return;
    }

    auto gameObject = GetGameObjectBasedOnParam(v[1]);

    if (not gameObject)
    {
        DEBUG_LOG("GameObject not found. " + v[1]);
        return;
    }

    for (auto &component : gameObject->GetComponents())
    {
        NewComponentMsgInd componentNameMsg(std::to_string(component->GetType()));
        componentNameMsg.isActive_ = component->IsActive();
        gateway_.Send(userId_, componentNameMsg);
    }
}

void NetworkEditorInterface::SetGameObjectPosition(const std::vector<std::string> &param)
{
    if (param.size() < 5)
    {
        DEBUG_LOG("param is empty");
        return;
    }

    std::unordered_map<std::string, std::string> v;

    for (const auto &p : param)
    {
        auto [name, value] = GetParamFromCommand(p);
        v.insert({name, value});
    }

    if (v.count("x") and v.count("y") and v.count("z"))
    {
        auto gameObject = GetGameObjectBasedOnParam(param[1]);
        if (gameObject)
        {
            try
            {
                vec3 position(std::stof(v.at("x")), std::stof(v.at("y")), std::stof(v.at("z")));

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
            catch (...)
            {
                ERROR_LOG("Set position error");
            }
        }
    }
}

void NetworkEditorInterface::SetGameObjectRotation(const std::vector<std::string> &param)
{
    if (param.size() < 5)
    {
        DEBUG_LOG("param is empty");
        return;
    }

    std::unordered_map<std::string, std::string> v;
    if (v.count("x") and v.count("y") and v.count("z"))
    {
        auto gameObject = GetGameObjectBasedOnParam(param[1]);
        if (gameObject)
        {
            try
            {
                vec3 rotation(std::stof(v.at("x")), std::stof(v.at("y")), std::stof(v.at("z")));

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
            catch (...)
            {
                ERROR_LOG("Set rotation error");
            }
        }
    }
}

void NetworkEditorInterface::SetGameObjectScale(const std::vector<std::string> &param)
{
    if (param.size() < 5)
    {
        DEBUG_LOG("param is empty");
        return;
    }

    std::unordered_map<std::string, std::string> v;
    for (const auto &p : param)
    {
        auto [name, value] = GetParamFromCommand(p);
        v.insert({name, value});
    }

    if (v.count("x") and v.count("y") and v.count("z"))
    {
        auto gameObject = GetGameObjectBasedOnParam(param[1]);
        if (gameObject)
        {
            try
            {
                vec3 scale(std::stof(v.at("x")), std::stof(v.at("y")), std::stof(v.at("z")));
                gameObject->worldTransform.SetScale(scale);
                gameObject->worldTransform.TakeSnapShoot();
            }
            catch (...)
            {
                ERROR_LOG("Set scale error");
            }
        }
    }
}

void NetworkEditorInterface::CreateGameObject(const std::vector<std::string> &params)
{
    if (params.empty())
    {
        DEBUG_LOG("param is empty");
        return;
    }

    std::string goName("NewGameObject");
    if (params.size() > 1)
    {
        goName = params[1];
    }

    auto go = scene_.CreateGameObject(goName);
    scene_.AddGameObject(go);
}

void NetworkEditorInterface::GetComponentsList(const std::vector<std::string> &)
{
    for (auto type : Components::GetComponentTypes())
    {
        AvailableComponentMsgInd msg(std::to_string(type));
        gateway_.Send(userId_, msg);
    }
}

void NetworkEditorInterface::AddComponent(const std::vector<std::string> &params)
{
    if (params.size() < 3)
    {
        DEBUG_LOG("param is empty");
        return;
    }
    auto map = CreateParamMap(params);

    if (map.count("id") > 0 and map.count("name") > 0)
    {
        auto &idParam       = map.at("id");
        auto &componentName = map.at("name");

        auto objectId = std::stoi(idParam);
        auto go       = scene_.GetGameObject(objectId);

        if (not go)
        {
            ERROR_LOG("Object not found with id : " + idParam);
            return;
        }

        auto componentType = std::from_string(componentName);

        if (componentType)
        {
            auto component = go->AddComponent(*componentType);
            NewComponentMsgInd componentNameMsg(componentName);
            componentNameMsg.isActive_ = component->IsActive();
            gateway_.Send(userId_, componentNameMsg);
        }
    }
}

void NetworkEditorInterface::StartScene(const std::vector<std::string> &)
{
    scene_.Start();
}

void NetworkEditorInterface::StopScene(const std::vector<std::string> &)
{
    scene_.Stop();
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

GameObject *NetworkEditorInterface::GetGameObjectBasedOnParam(const std::string &param)
{
    if (param.empty())
    {
        DEBUG_LOG("param is empty");
        return nullptr;
    }

    auto [name, value] = GetParamFromCommand(param);

    if (name != "id")
    {
        DEBUG_LOG("param id not found.");
        return nullptr;
    }

    auto objectId = std::stoi(value);
    return scene_.GetGameObject(objectId);
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
