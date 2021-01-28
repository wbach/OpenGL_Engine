#include "RemoveComponentMsgIndXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const RemoveComponentMsgInd& input)
{
    auto root = std::make_unique<TreeNode>("RemoveComponentMsgInd");
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
