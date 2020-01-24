#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"
#include "TransformMessageTypes.h"

namespace common
{
struct TransformMsgReq : public Network::IMessage
{
    TransformMsgReq()
        : IMessage(MessageTypes::TransformReq, Network::MessageTarget::All)
    {
    }
    TransformMessageTypes type = TransformMessageTypes::NONE;
    TransformAction action     = TransformAction::POP;
    uint32 id                  = 0;

    virtual std::string ToString() const override
    {
        return "ID: " + std::to_string(id) + "\nType: " + std::to_string(type) + "\nAction: " + std::to_string(action);
    }
};
}  // namespace Network
