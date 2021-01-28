#include "TransformXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(Transform& output, TreeNode& input)
{
    if (input.isAttributePresent("objectId"))
    {
        output.objectId = std::stoi(input.attributes_.at("objectId"));
    }
    if (input.isAttributePresent("position"))
    {
        output.position = ::ConvertToVec3(*input.getChild("position"));
    }
    if (input.isAttributePresent("rotation"))
    {
        output.rotation = ::ConvertToVec3(*input.getChild("rotation"));
    }
    if (input.isAttributePresent("scale"))
    {
        output.scale = ::ConvertToVec3(*input.getChild("scale"));
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
