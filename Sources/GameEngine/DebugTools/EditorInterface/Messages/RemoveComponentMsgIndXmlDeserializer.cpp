#include "RemoveComponentMsgIndXmlDeserializer.h"
#include <Utils/XML/XMLUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(RemoveComponentMsgInd& output, TreeNode& input)
{
    if (input.isAttributePresent("name"))
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
