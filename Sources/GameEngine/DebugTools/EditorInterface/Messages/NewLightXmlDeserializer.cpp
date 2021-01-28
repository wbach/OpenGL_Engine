#include "NewLightXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewLight& output, TreeNode& input)
{
    if (input.isAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
    if (input.isAttributePresent("type"))
    {
        output.type = std::stoi(input.attributes_.at("type"));
    }
    if (input.isAttributePresent("position"))
    {
        output.position = ::ConvertToVec3(*input.getChild("position"));
    }
    if (input.isAttributePresent("colour"))
    {
        output.colour = ::ConvertToVec3(*input.getChild("colour"));
    }
    if (input.isAttributePresent("attenuation"))
    {
        output.attenuation = ::ConvertToVec3(*input.getChild("attenuation"));
    }
    if (input.isAttributePresent("cutOff"))
    {
        output.cutOff = std::stof(input.attributes_.at("cutOff"));
    }
    if (input.isAttributePresent("direction"))
    {
        output.direction = ::ConvertToVec3(*input.getChild("direction"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeNewLight(Utils::XmlReader& reader)
{
    auto result = std::make_unique<NewLight>();
    auto msg = reader.Get("NewLight");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
