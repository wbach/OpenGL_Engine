#include "NetworkEditorInterface.h"

#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Messages/GameObjectMsg.h"
#include "Messages/TransformMsg.h"
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

    gateway_.AddMessageConverter(std::make_unique<GameEngine::XmlMessageConverter>());

    DEBUG_LOG("Starting server");
    gateway_.StartServer(30, 1991);
    gateway_.SetDefaultMessageConverterFormat(Network::MessageFormat::Xml);
    gateway_.SubscribeForNewUser(std::bind(&NetworkEditorInterface::NewUser, this, std::placeholders::_1, std::placeholders::_2));
    gateway_.SubscribeForDisconnectUser(std::bind(&NetworkEditorInterface::DisconnectUser, this, std::placeholders::_1));
    gateway_.SubscribeOnMessageArrived(Network::MessageTypes::Text, std::bind(&NetworkEditorInterface::OnMessage, this, std::placeholders::_1, std::placeholders::_2));

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

void SendChildrenObjectList(uint32 userId, Network::Gateway &gateway, uint32 parentId, const std::vector<std::unique_ptr<GameObject>> &objectList)
{
    if (objectList.empty())
    {
        DEBUG_LOG("No children found. Parent id : " + std::to_string(parentId));
        return;
    }

    for (auto &go : objectList)
    {
        GameObjectMsg message(go->GetName());
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
            GameObjectMsg message(go.second->GetName());
            message.id = go.second->GetId();
            gateway_.Send(userId_, message);
            SendChildrenObjectList(userId_, gateway_, go.second->GetId(), go.second->GetChildrens());
        }
    }

    DEBUG_LOG("");
    Network::TextMessage endMsg("end");
    gateway_.Send(userId_, endMsg);
}

void NetworkEditorInterface::TransformReq(const std::vector<std::string> & v)
{
    if (v.empty())
    {
        DEBUG_LOG("object id not found.");
        return;
    }

    auto& param = v[1];
    for(auto& p : v)
        DEBUG_LOG(p);
    DEBUG_LOG("--");
    DEBUG_LOG(param);
    auto name = param.substr(0, param.find_first_of('='));
    DEBUG_LOG(name);
    if (name != "id")
    {
        DEBUG_LOG("param id not found.");
        return;
    }

    auto value = param.substr(param.find_first_of('=') +1);
    DEBUG_LOG("Get object id : " + value);
    auto objectId = std::stoi(value);
    auto go = scene_.GetGameObject(objectId);

    if (not go)
    {
        DEBUG_LOG("GameObject not found. Id="+value);
        return;
    }

    auto& transform = go->worldTransform;

    TransformMsg msg(objectId, transform.GetPosition(), transform.GetRotation(), transform.GetScale());
    gateway_.Send(userId_, msg);
}
}  // namespace GameEngine
