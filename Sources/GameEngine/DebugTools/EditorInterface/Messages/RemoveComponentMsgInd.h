#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct RemoveComponentMsgInd : public Network::IMessage
{
    RemoveComponentMsgInd()
    : IMessage(Convert(MessageTypes::RemoveComponentMsgInd), Network::MessageTarget::All)
    {
    }
    RemoveComponentMsgInd(
            const std::string& name)
    : IMessage(Convert(MessageTypes::RemoveComponentMsgInd), Network::MessageTarget::All)
    , name{name}
    {
    }

    std::string name;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
