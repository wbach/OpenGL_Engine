#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct SceneStartedNotifMsg : public Network::IMessage
{
    SceneStartedNotifMsg()
    : IMessage(Convert(MessageTypes::SceneStartedNotifMsg), Network::MessageTarget::All)
    {
    }
    SceneStartedNotifMsg(
            const std::string& name)
    : IMessage(Convert(MessageTypes::SceneStartedNotifMsg), Network::MessageTarget::All)
    , name{name}
    {
    }

    std::string name;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
