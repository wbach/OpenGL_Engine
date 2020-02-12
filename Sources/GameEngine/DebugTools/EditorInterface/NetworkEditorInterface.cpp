#include "NetworkEditorInterface.h"

#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/NewComponentMsgInd.h"
#include "Messages/NewGameObjectIndMsg.h"
#include "Messages/TransformMsg.h"
#include "Messages/RemoveComponentMsgInd.h"
#include "Messages/RemoveGameObjectIndMsg.h"
#include "XMLMessageConverter.h"

namespace GameEngine
{
NetworkEditorInterface::NetworkEditorInterface(Scene &scene)
    : scene_(scene)
    , isRunning_{true}
    , userId_{0}
{
    commands_.insert({"openFile", [&](const std::vector<std::string> &v) { LoadSceneFromFile(v); }});
    commands_.insert({"getObjectList", [&](const std::vector<std::string> &v) { GetObjectList(v); }});
    commands_.insert({"transformReq", [&](const std::vector<std::string> &v) { TransformReq(v); }});
    commands_.insert({"getGameObjectComponentsListReq",
                      [&](const std::vector<std::string> &v) { GetGameObjectComponentsListReq(v); }});
    commands_.insert({"setPosition", [&](const std::vector<std::string> &v) { SetGameObjectPosition(v); }});
    commands_.insert({"setRotation", [&](const std::vector<std::string> &v) { SetGameObjectRotation(v); }});
    commands_.insert({"setScale", [&](const std::vector<std::string> &v) { SetGameObjectScale(v); }});

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

    if (transformChangeSubscriptionId_)
    {
        if (not transformChangeSubscription_)
        {
            ERROR_LOG("Somthing went wrong. transformChangeSubscription_ is nullptr");
            return;
        }

        transformChangeSubscription_->UnsubscribeOnChange(*transformChangeSubscriptionId_);
    }

    auto &transform                = gameObject->worldTransform;
    transformChangeSubscription_   = &transform;
    transformChangeSubscriptionId_ = transform.SubscribeOnChange([&](const auto &transform) {
        const auto &t = transform.GetSnapShoot();
        TransformMsg msg(gameObject->GetId(), t.position, t.rotation, t.scale);
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
}  // namespace GameEngine
