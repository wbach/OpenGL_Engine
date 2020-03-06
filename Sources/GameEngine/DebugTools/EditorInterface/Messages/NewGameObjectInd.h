#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct NewGameObjectInd : public Network::IMessage
{
    NewGameObjectInd()
    : IMessage(Convert(MessageTypes::NewGameObjectInd), Network::MessageTarget::All)
    {
    }
    NewGameObjectInd(
            uint32 id,
            uint32 parentId,
            const std::string& name)
    : IMessage(Convert(MessageTypes::NewGameObjectInd), Network::MessageTarget::All)
    , id{id}
    , parentId{parentId}
    , name{name}
    {
    }

    uint32 id;
    uint32 parentId;
    std::string name;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
