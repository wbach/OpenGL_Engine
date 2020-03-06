#include "CameraMsgXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(CameraMsg& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("position"))
    {
        output.position = Utils::ConvertToVec3(*input.GetChild("position"));
    }
    if (input.IsAttributePresent("rotation"))
    {
        output.rotation = Utils::ConvertToVec3(*input.GetChild("rotation"));
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
