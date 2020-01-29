#pragma once
#include "Common/MessageHandling/AbstractHandler.h"

namespace common
{
struct TransformMsgResp;
}

namespace MmmoRpg
{
class NetworkCharacterManager;

class TransformHandler : public common::AbstractHandler
{
public:
    TransformHandler(NetworkCharacterManager& networkCharacterManager)
        : common::AbstractHandler({common::MessageTypes::TransformResp})
        , networkCharacterManager_(networkCharacterManager)
    {
    }

protected:
    virtual void ProcessMessage(Network::UserId, const Network::IMessage &message) override;
    void HandleTransformMsg(const common::TransformMsgResp& msg);
    NetworkCharacterManager& networkCharacterManager_;
};

}  // MmmoRpg
