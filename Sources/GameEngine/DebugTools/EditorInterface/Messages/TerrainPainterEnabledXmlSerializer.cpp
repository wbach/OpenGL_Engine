#include "TerrainPainterEnabledXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const TerrainPainterEnabled& input)
{
    auto root = std::make_unique<Utils::XmlNode>("TerrainPainterEnabled");
    root->AddChild(Utils::Convert("brushTypes", input.brushTypes));
    root->AddChild(Utils::Convert("stepInterpolations", input.stepInterpolations));
    root->attributes_.insert({"selectedBrushType", input.selectedBrushType});
    root->attributes_.insert({"type", input.type});
    root->attributes_.insert({"stepInterpolation", input.stepInterpolation});
    root->attributes_.insert({"strength", std::to_string(input.strength)});
    root->attributes_.insert({"brushSize", std::to_string(input.brushSize)});
    return root;
}
Network::IMessageData Serialize(const TerrainPainterEnabled& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
