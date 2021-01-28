#include "CameraMsgXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(CameraMsg& output, TreeNode& input)
{
    if (input.isAttributePresent("position"))
    {
        output.position = ::ConvertToVec3(*input.getChild("position"));
    }
    if (input.isAttributePresent("rotation"))
    {
        output.rotation = ::ConvertToVec3(*input.getChild("rotation"));
    }
}
std::unique_ptr<Network::IMessage> DeserializeCameraMsg(Utils::XmlReader& reader)
{
    auto result = std::make_unique<CameraMsg>();
    auto msg = reader.Get("CameraMsg");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
