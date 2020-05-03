#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct TerrainPainterEnabled : public Network::IMessage
{
    TerrainPainterEnabled()
    : IMessage(Convert(MessageTypes::TerrainPainterEnabled), Network::MessageTarget::All)
    {
    }
    TerrainPainterEnabled(
            const std::vector<std::string>& brushTypes,
            const std::vector<std::string>& stepInterpolations,
            const std::string& selectedBrushType,
            const std::string& type,
            const std::string& stepInterpolation,
            float strength,
            int brushSize)
    : IMessage(Convert(MessageTypes::TerrainPainterEnabled), Network::MessageTarget::All)
    , brushTypes{brushTypes}
    , stepInterpolations{stepInterpolations}
    , selectedBrushType{selectedBrushType}
    , type{type}
    , stepInterpolation{stepInterpolation}
    , strength{strength}
    , brushSize{brushSize}
    {
    }

    std::vector<std::string> brushTypes;
    std::vector<std::string> stepInterpolations;
    std::string selectedBrushType;
    std::string type;
    std::string stepInterpolation;
    float strength;
    int brushSize;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
