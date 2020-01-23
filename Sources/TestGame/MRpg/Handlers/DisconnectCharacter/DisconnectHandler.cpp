#include "DisconnectHandler.h"
#include <Common/Messages/RemoveCharacter/DisconnectCharacterMsg.h>
#include <TestGame/MRpg/Characters/NetworkCharacterManger.h>

namespace MmmoRpg
{
void DisconnectHandler::ProcessMessage(const Network::IMessage& message)
{
    auto msg = static_cast<const common::DisconnectCharacterMsg*>(&message);

    if (msg == nullptr)
    {
        DEBUG_LOG("Got msg but wrong type : " + std::to_string(message.GetType()));
        return;
    }

    networkCharacterManager_.RemoveCharacter(msg->id);
}

}  // namespace MmmoRpg
