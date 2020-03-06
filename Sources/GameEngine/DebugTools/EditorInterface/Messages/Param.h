#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct Param : public Network::IMessage
{
    Param()
    : IMessage(Convert(MessageTypes::Param), Network::MessageTarget::All)
    {
    }
    Param(
            const std::string& name,
            const std::string& value,
            const std::string& type)
    : IMessage(Convert(MessageTypes::Param), Network::MessageTarget::All)
    , name{name}
    , value{value}
    , type{type}
    {
    }

    std::string name;
    std::string value;
    std::string type;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
