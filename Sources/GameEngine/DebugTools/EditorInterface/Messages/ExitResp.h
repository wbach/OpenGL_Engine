#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct ExitResp : public Network::IMessage
{
    ExitResp()
    : IMessage(Convert(MessageTypes::ExitResp), Network::MessageTarget::All)
    {
    }
    ExitResp(
            uint32 status)
    : IMessage(Convert(MessageTypes::ExitResp), Network::MessageTarget::All)
    , status{status}
    {
    }

    uint32 status;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
