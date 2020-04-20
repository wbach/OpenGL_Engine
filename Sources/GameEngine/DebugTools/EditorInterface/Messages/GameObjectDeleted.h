#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct GameObjectDeleted : public Network::IMessage
{
    GameObjectDeleted()
    : IMessage(Convert(MessageTypes::GameObjectDeleted), Network::MessageTarget::All)
    {
    }
    GameObjectDeleted(
            uint32 gameObjectId)
    : IMessage(Convert(MessageTypes::GameObjectDeleted), Network::MessageTarget::All)
    , gameObjectId{gameObjectId}
    {
    }

    uint32 gameObjectId;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
