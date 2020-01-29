#include "NetworkEditorInterface.h"

#include <Utils.h>
#include <UtilsNetwork/Messages/TextMessage.h>

#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
NetworkEditorInterface::NetworkEditorInterface(Scene &scene)
    : scene_(scene)
    , isRunning_{true}
{
    commands_.insert({"openFile", [&](const std::vector<std::string> &v) { LoadSceneFromFile(v); }});

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
}  // namespace GameEngine
