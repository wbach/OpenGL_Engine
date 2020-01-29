#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"
#include "TransformMessageTypes.h"

namespace common
{
struct TransformMsgResp : public Network::IMessage
{
    TransformMsgResp()
        : IMessage(MessageTypes::TransformResp, Network::MessageTarget::All)
    {
    }
    TransformMessageTypes type = TransformMessageTypes::NONE;
    TransformAction action     = TransformAction::POP;
    uint32 id                  = 0;
    vec3 position;
    vec3 rotation;

    virtual std::string ToString() const override
    {
        return "ID: " + std::to_string(id) + "\nType: " + std::to_string(type) + "\nAction: " + std::to_string(action) +
               "\nPosition: " + std::to_string(position) + "\nRotation : " + std::to_string(rotation);
    }
};
}  // namespace common
