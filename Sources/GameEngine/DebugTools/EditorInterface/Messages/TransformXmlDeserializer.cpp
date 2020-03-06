#include "TransformXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(Transform& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("objectId"))
    {
        output.objectId = std::stoi(input.attributes_.at("objectId"));
    }
    if (input.IsAttributePresent("position"))
    {
        output.position = Utils::ConvertToVec3(*input.GetChild("position"));
    }
    if (input.IsAttributePresent("rotation"))
    {
        output.rotation = Utils::ConvertToVec3(*input.GetChild("rotation"));
    }
    if (input.IsAttributePresent("scale"))
    {
        output.scale = Utils::ConvertToVec3(*input.GetChild("scale"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeTransform(Utils::XmlReader& reader)
{
    auto result = std::make_unique<Transform>();
    auto msg = reader.Get("Transform");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
