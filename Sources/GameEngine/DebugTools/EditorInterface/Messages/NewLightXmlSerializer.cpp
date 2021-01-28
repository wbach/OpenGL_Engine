#include "NewLightXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const NewLight& input)
{
    auto root = std::make_unique<TreeNode>("NewLight");
    root->attributes_.insert({"id", std::to_string(input.id)});
    root->attributes_.insert({"type", std::to_string(input.type)});
    root->addChild(::Convert("position", input.position));
    root->addChild(::Convert("colour", input.colour));
    root->addChild(::Convert("attenuation", input.attenuation));
    root->attributes_.insert({"cutOff", std::to_string(input.cutOff)});
    root->addChild(::Convert("direction", input.direction));
    return root;
}
Network::IMessageData Serialize(const NewLight& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
