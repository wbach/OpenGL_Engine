#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct SceneFileMsg : public Network::IMessage
{
    SceneFileMsg()
    : IMessage(Convert(MessageTypes::SceneFileMsg), Network::MessageTarget::All)
    {
    }
    SceneFileMsg(
            const std::string& filename)
    : IMessage(Convert(MessageTypes::SceneFileMsg), Network::MessageTarget::All)
    , filename{filename}
    {
    }

    std::string filename;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
