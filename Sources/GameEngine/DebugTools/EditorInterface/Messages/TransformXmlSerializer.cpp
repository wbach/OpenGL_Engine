#include "TransformXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const Transform& input)
{
    auto root = std::make_unique<TreeNode>("Transform");
    root->attributes_.insert({"objectId", std::to_string(input.objectId)});
    root->addChild(::Convert("position", input.position));
    root->addChild(::Convert("rotation", input.rotation));
    root->addChild(::Convert("scale", input.scale));
    return root;
}
Network::IMessageData Serialize(const Transform& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
