#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct RemoveGameObjectInd : public Network::IMessage
{
    RemoveGameObjectInd()
    : IMessage(Convert(MessageTypes::RemoveGameObjectInd), Network::MessageTarget::All)
    {
    }
    RemoveGameObjectInd(
            uint32 id)
    : IMessage(Convert(MessageTypes::RemoveGameObjectInd), Network::MessageTarget::All)
    , id{id}
    {
    }

    uint32 id;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
