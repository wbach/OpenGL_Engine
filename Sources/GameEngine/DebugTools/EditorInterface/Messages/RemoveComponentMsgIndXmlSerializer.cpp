#include "RemoveComponentMsgIndXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const RemoveComponentMsgInd& input)
{
    auto root = std::make_unique<Utils::XmlNode>("RemoveComponentMsgInd");
    root->attributes_.insert({"name", input.name});
    return root;
}
Network::IMessageData Serialize(const RemoveComponentMsgInd& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
