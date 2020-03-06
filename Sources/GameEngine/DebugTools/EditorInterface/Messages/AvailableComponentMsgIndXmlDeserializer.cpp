#include "AvailableComponentMsgIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(AvailableComponentMsgInd& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
}
std::unique_ptr<Network::IMessage> DeserializeAvailableComponentMsgInd(Utils::XmlReader& reader)
{
    auto result = std::make_unique<AvailableComponentMsgInd>();
    auto msg = reader.Get("AvailableComponentMsgInd");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
