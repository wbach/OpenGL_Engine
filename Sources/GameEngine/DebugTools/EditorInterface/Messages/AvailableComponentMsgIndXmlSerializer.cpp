#include "AvailableComponentMsgIndXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const AvailableComponentMsgInd& input)
{
    auto root = std::make_unique<Utils::XmlNode>("AvailableComponentMsgInd");
    root->attributes_.insert({"name", input.name});
    return root;
}
Network::IMessageData Serialize(const AvailableComponentMsgInd& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
