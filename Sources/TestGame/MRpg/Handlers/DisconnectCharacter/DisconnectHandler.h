#pragma once
#include "Common/MessageHandling/AbstractHandler.h"

namespace MmmoRpg
{
class NetworkCharacterManager;

class DisconnectHandler : public common::AbstractHandler
{
   public:
    DisconnectHandler(NetworkCharacterManager& networkCharacterManager)
        : common::AbstractHandler({common::MessageTypes::DisconnectCharacter})
        , networkCharacterManager_(networkCharacterManager)
    {
    }

   protected:
    virtual void ProcessMessage(Network::UserId userId, const Network::IMessage& message) override;
    NetworkCharacterManager& networkCharacterManager_;
};
}  // namespace MmmoRpg
