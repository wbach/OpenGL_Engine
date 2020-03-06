#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct AvailableComponentMsgInd : public Network::IMessage
{
    AvailableComponentMsgInd()
    : IMessage(Convert(MessageTypes::AvailableComponentMsgInd), Network::MessageTarget::All)
    {
    }
    AvailableComponentMsgInd(
            const std::string& name)
    : IMessage(Convert(MessageTypes::AvailableComponentMsgInd), Network::MessageTarget::All)
    , name{name}
    {
    }

    std::string name;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
