#include "NewComponentMsgIndXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const NewComponentMsgInd& input)
{
    auto root = std::make_unique<Utils::XmlNode>("NewComponentMsgInd");
    root->attributes_.insert({"name", input.name});
    root->attributes_.insert({"isActive", std::to_string(input.isActive)});
    return root;
}
Network::IMessageData Serialize(const NewComponentMsgInd& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
