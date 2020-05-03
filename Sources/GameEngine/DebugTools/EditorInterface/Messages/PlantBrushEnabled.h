#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct PlantBrushEnabled : public Network::IMessage
{
    PlantBrushEnabled()
    : IMessage(Convert(MessageTypes::PlantBrushEnabled), Network::MessageTarget::All)
    {
    }
    PlantBrushEnabled(
            const std::vector<std::string>& brushTypes,
            const std::string& texture,
            int brushSize)
    : IMessage(Convert(MessageTypes::PlantBrushEnabled), Network::MessageTarget::All)
    , brushTypes{brushTypes}
    , texture{texture}
    , brushSize{brushSize}
    {
    }

    std::vector<std::string> brushTypes;
    std::string texture;
    int brushSize;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
