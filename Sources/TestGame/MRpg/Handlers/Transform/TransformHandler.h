#pragma once
#include "Common/MessageHandling/AbstractHandler.h"

namespace Network
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
        : common::AbstractHandler({Network::MessageTypes::TransformResp})
        , networkCharacterManager_(networkCharacterManager)
    {
    }

protected:
    virtual void ProcessMessage(const Network::BoxMessage& message) override;
    void HandleTransformMsg(const Network::TransformMsgResp& msg);
    NetworkCharacterManager& networkCharacterManager_;
};

}  // MmmoRpg
