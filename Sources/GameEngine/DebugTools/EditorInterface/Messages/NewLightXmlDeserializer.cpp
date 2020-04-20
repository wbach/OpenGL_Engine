#include "NewLightXmlDeserializer.h"

#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewLight& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("id"))
    {
        output.id = std::stoi(input.attributes_.at("id"));
    }
    if (input.IsAttributePresent("type"))
    {
        output.type = std::stoi(input.attributes_.at("type"));
    }
    if (input.IsAttributePresent("position"))
    {
        output.position = Utils::ConvertToVec3(*input.GetChild("position"));
    }
    if (input.IsAttributePresent("colour"))
    {
        output.colour = Utils::ConvertToVec3(*input.GetChild("colour"));
    }
    if (input.IsAttributePresent("attenuation"))
    {
        output.attenuation = Utils::ConvertToVec3(*input.GetChild("attenuation"));
    }
    if (input.IsAttributePresent("cutOff"))
    {
        output.cutOff = std::stof(input.attributes_.at("cutOff"));
    }
    if (input.IsAttributePresent("direction"))
    {
        output.direction = Utils::ConvertToVec3(*input.GetChild("direction"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeNewLight(Utils::XmlReader& reader)
{
    auto result = std::make_unique<NewLight>();
    auto msg    = reader.Get("NewLight");
    if (not msg)
        return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
}  // namespace DebugNetworkInterface
}  // namespace GameEngine
