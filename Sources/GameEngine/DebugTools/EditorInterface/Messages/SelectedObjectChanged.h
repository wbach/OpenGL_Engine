#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct SelectedObjectChanged : public Network::IMessage
{
    SelectedObjectChanged()
    : IMessage(Convert(MessageTypes::SelectedObjectChanged), Network::MessageTarget::All)
    {
    }
    SelectedObjectChanged(
            uint32 id)
    : IMessage(Convert(MessageTypes::SelectedObjectChanged), Network::MessageTarget::All)
    , id{id}
    {
    }

    uint32 id;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
