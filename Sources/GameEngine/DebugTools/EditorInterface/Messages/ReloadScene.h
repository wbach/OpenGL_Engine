#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct ReloadScene : public Network::IMessage
{
    ReloadScene()
    : IMessage(Convert(MessageTypes::ReloadScene), Network::MessageTarget::All)
    {
    }
    ReloadScene(
            const std::string& filename)
    : IMessage(Convert(MessageTypes::ReloadScene), Network::MessageTarget::All)
    , filename{filename}
    {
    }

    std::string filename;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
