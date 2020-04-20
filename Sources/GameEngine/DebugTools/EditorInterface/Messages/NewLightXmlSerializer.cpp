#include "NewLightXmlSerializer.h"
#include <Utils.h>
#include <Utils/XML/XMLUtils.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const NewLight& input)
{
    auto root = std::make_unique<Utils::XmlNode>("NewLight");
    root->attributes_.insert({"id", std::to_string(input.id)});
    root->attributes_.insert({"type", std::to_string(input.type)});
    root->AddChild(std::move(Utils::Convert("position", input.position)));
    root->AddChild(std::move(Utils::Convert("colour", input.colour)));
    root->AddChild(std::move(Utils::Convert("attenuation", input.attenuation)));
    root->attributes_.insert({"cutOff", std::to_string(input.cutOff)});
    root->AddChild(std::move(Utils::Convert("direction", input.direction)));
    return root;
}
Network::IMessageData Serialize(const NewLight& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
