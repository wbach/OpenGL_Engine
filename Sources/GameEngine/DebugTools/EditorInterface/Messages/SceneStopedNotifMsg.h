#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct SceneStopedNotifMsg : public Network::IMessage
{
    SceneStopedNotifMsg()
    : IMessage(Convert(MessageTypes::SceneStopedNotifMsg), Network::MessageTarget::All)
    {
    }
    SceneStopedNotifMsg(
            const std::string& name)
    : IMessage(Convert(MessageTypes::SceneStopedNotifMsg), Network::MessageTarget::All)
    , name{name}
    {
    }

    std::string name;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
