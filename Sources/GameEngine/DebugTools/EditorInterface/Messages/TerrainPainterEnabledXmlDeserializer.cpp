#include "TerrainPainterEnabledXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(TerrainPainterEnabled& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("brushTypes"))
    {
        for (auto& child : input.GetChild("brushTypes")->GetChildren())
        {
            output.brushTypes.push_back(child->value_);
        }
    }
    if (input.IsAttributePresent("stepInterpolations"))
    {
        for (auto& child : input.GetChild("stepInterpolations")->GetChildren())
        {
            output.stepInterpolations.push_back(child->value_);
        }
    }
    if (input.IsAttributePresent("selectedBrushType"))
    {
        output.selectedBrushType = input.attributes_.at("selectedBrushType");
    }
    if (input.IsAttributePresent("type"))
    {
        output.type = input.attributes_.at("type");
    }
    if (input.IsAttributePresent("stepInterpolation"))
    {
        output.stepInterpolation = input.attributes_.at("stepInterpolation");
    }
    if (input.IsAttributePresent("strength"))
    {
        output.strength = std::stof(input.attributes_.at("strength"));
    }
    if (input.IsAttributePresent("brushSize"))
    {
        output.brushSize = std::stoi(input.attributes_.at("brushSize"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeTerrainPainterEnabled(Utils::XmlReader& reader)
{
    auto result = std::make_unique<TerrainPainterEnabled>();
    auto msg = reader.Get("TerrainPainterEnabled");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
