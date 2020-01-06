#include "NetworkEditorInterface.h"

#include <UtilsNetwork/Messages/Other/OtherMsg.h>

namespace GameEngine
{
NetworkEditorInterface::NetworkEditorInterface()
{
    DEBUG_LOG("Starting server...");
    gateway_.StartServer(30, 1991);
    gateway_.SubscribeForNewUser(
        std::bind(&NetworkEditorInterface::NewUser, this, std::placeholders::_1, std::placeholders::_2));
    gateway_.SubscribeForDisconnectUser(
        std::bind(&NetworkEditorInterface::DisconnectUser, this, std::placeholders::_1));
    gateway_.SubscribeOnMessageArrived("Dispat",
                                       std::bind(&NetworkEditorInterface::OnMessage, this, std::placeholders::_1));
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
void NetworkEditorInterface::OnMessage(const Network::BoxMessage &message)
{
    if (message.second->GetType() == Network::MessageTypes::Other)
    {
        auto msg = Network::castMessageAs<Network::OtherMsg>(message.second);
        if (msg)
        {
            DEBUG_LOG("Message recevied : " + msg->GetData());
        }
    }
    else
    {
        DEBUG_LOG("Unknown msg recevied");
    }
}
}  // namespace GameEngine
