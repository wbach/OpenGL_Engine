#include "TransformXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const Transform& input)
{
    auto root = std::make_unique<Utils::XmlNode>("Transform");
    root->attributes_.insert({"objectId", std::to_string(input.objectId)});
    root->AddChild(std::move(Utils::Convert("position", input.position)));
    root->AddChild(std::move(Utils::Convert("rotation", input.rotation)));
    root->AddChild(std::move(Utils::Convert("scale", input.scale)));
    return root;
}
Network::IMessageData Serialize(const Transform& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
