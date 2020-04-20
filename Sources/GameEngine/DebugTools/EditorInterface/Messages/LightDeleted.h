#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct LightDeleted : public Network::IMessage
{
    LightDeleted()
    : IMessage(Convert(MessageTypes::LightDeleted), Network::MessageTarget::All)
    {
    }
    LightDeleted(
            uint32 id)
    : IMessage(Convert(MessageTypes::LightDeleted), Network::MessageTarget::All)
    , id{id}
    {
    }

    uint32 id;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
