#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct ComponentDeleted : public Network::IMessage
{
    ComponentDeleted()
    : IMessage(Convert(MessageTypes::ComponentDeleted), Network::MessageTarget::All)
    {
    }
    ComponentDeleted(
            uint32 gameObjectId,
            const std::string& componentName)
    : IMessage(Convert(MessageTypes::ComponentDeleted), Network::MessageTarget::All)
    , gameObjectId{gameObjectId}
    , componentName{componentName}
    {
    }

    uint32 gameObjectId;
    std::string componentName;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
