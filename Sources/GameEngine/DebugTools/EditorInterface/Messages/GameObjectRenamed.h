#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct GameObjectRenamed : public Network::IMessage
{
    GameObjectRenamed()
    : IMessage(Convert(MessageTypes::GameObjectRenamed), Network::MessageTarget::All)
    {
    }
    GameObjectRenamed(
            uint32 gameObjectId,
            const std::string& newName)
    : IMessage(Convert(MessageTypes::GameObjectRenamed), Network::MessageTarget::All)
    , gameObjectId{gameObjectId}
    , newName{newName}
    {
    }

    uint32 gameObjectId;
    std::string newName;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
