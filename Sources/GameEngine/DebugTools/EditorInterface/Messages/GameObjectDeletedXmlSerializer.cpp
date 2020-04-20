#include "GameObjectDeletedXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const GameObjectDeleted& input)
{
    auto root = std::make_unique<Utils::XmlNode>("GameObjectDeleted");
    root->attributes_.insert({"gameObjectId", std::to_string(input.gameObjectId)});
    return root;
}
Network::IMessageData Serialize(const GameObjectDeleted& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
