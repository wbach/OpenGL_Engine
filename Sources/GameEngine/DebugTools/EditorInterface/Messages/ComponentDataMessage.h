#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"
#include "Param.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct ComponentDataMessage : public Network::IMessage
{
    ComponentDataMessage()
    : IMessage(Convert(MessageTypes::ComponentDataMessage), Network::MessageTarget::All)
    {
    }
    ComponentDataMessage(
            const std::string& name,
            uint32 gameObjectId,
            const std::vector<Param>& params)
    : IMessage(Convert(MessageTypes::ComponentDataMessage), Network::MessageTarget::All)
    , name{name}
    , gameObjectId{gameObjectId}
    , params{params}
    {
    }

    std::string name;
    uint32 gameObjectId;
    std::vector<Param> params;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
