#include "TerrainPainterEnabledXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(TerrainPainterEnabled& output, TreeNode& input)
{
    if (input.isAttributePresent("brushTypes"))
    {
        for (auto& child : input.getChild("brushTypes")->getChildren())
        {
            output.brushTypes.push_back(child->value_);
        }
    }
    if (input.isAttributePresent("stepInterpolations"))
    {
        for (auto& child : input.getChild("stepInterpolations")->getChildren())
        {
            output.stepInterpolations.push_back(child->value_);
        }
    }
    if (input.isAttributePresent("selectedBrushType"))
    {
        output.selectedBrushType = input.attributes_.at("selectedBrushType");
    }
    if (input.isAttributePresent("type"))
    {
        output.type = input.attributes_.at("type");
    }
    if (input.isAttributePresent("stepInterpolation"))
    {
        output.stepInterpolation = input.attributes_.at("stepInterpolation");
    }
    if (input.isAttributePresent("strength"))
    {
        output.strength = std::stof(input.attributes_.at("strength"));
    }
    if (input.isAttributePresent("brushSize"))
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
