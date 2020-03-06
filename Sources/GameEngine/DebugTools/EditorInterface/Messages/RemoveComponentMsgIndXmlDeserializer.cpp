#include "RemoveComponentMsgIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(RemoveComponentMsgInd& output, Utils::XmlNode& input)
{
    if (input.IsAttributePresent("name"))
    {
        output.name = input.attributes_.at("name");
    }
}
std::unique_ptr<Network::IMessage> DeserializeRemoveComponentMsgInd(Utils::XmlReader& reader)
{
    auto result = std::make_unique<RemoveComponentMsgInd>();
    auto msg = reader.Get("RemoveComponentMsgInd");
    if (not msg) return nullptr;
    SetParam(*result, *msg);
    return std::move(result);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
