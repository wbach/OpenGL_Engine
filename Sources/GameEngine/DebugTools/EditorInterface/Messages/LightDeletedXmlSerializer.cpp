#include "LightDeletedXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const LightDeleted& input)
{
    auto root = std::make_unique<Utils::XmlNode>("LightDeleted");
    root->attributes_.insert({"id", std::to_string(input.id)});
    return root;
}
Network::IMessageData Serialize(const LightDeleted& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
