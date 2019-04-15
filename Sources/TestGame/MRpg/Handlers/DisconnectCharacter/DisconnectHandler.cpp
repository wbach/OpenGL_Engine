#include "DisconnectHandler.h"
#include "Messages/RemoveCharacter/DisconnectCharacterMsg.h"
#include "TestGame/MRpg/Characters/NetworkCharacterManger.h"

namespace MmmoRpg
{
void DisconnectHandler::ProcessMessage(const Network::BoxMessage& message)
{
    auto msg = Network::castMessageAs<Network::DisconnectCharacterMsg>(message.second.get());

    if (msg == nullptr)
    {
        DEBUG_LOG("Got msg but wrong type : " + std::to_string(msg->GetType()));
        return;
    }

    networkCharacterManager_.RemoveCharacter(msg->id);
}

}  // namespace MmmoRpg
