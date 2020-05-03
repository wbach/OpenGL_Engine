#include "PlantBrushEnabledXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(PlantBrushEnabled& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("brushTypes"))
    {
        for (auto& child : input.GetChild("brushTypes")->GetChildren())
        {
            output.brushTypes.push_back(child->value_);
        }
    }
    if (input.IsAttributePresent("texture"))
    {
        output.texture = input.attributes_.at("texture");
    }
    if (input.IsAttributePresent("brushSize"))
    {
        output.brushSize = std::stoi(input.attributes_.at("brushSize"));
    }
}
std::unique_ptr<Network::IMessage> DeserializePlantBrushEnabled(Utils::XmlReader& reader)
{
    auto result = std::make_unique<PlantBrushEnabled>();
    auto msg = reader.Get("PlantBrushEnabled");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
