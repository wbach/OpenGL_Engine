#include "ParamXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const Param& input)
{
    auto root = std::make_unique<TreeNode>("Param");
    root->attributes_.insert({"name", input.name});
    root->attributes_.insert({"value", input.value});
    root->attributes_.insert({"type", input.type});
    return root;
}
Network::IMessageData Serialize(const Param& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
