#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct NewComponentMsgInd : public Network::IMessage
{
    NewComponentMsgInd()
    : IMessage(Convert(MessageTypes::NewComponentMsgInd), Network::MessageTarget::All)
    {
    }
    NewComponentMsgInd(
            const std::string& name,
            bool isActive)
    : IMessage(Convert(MessageTypes::NewComponentMsgInd), Network::MessageTarget::All)
    , name{name}
    , isActive{isActive}
    {
    }

    std::string name;
    bool isActive;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
