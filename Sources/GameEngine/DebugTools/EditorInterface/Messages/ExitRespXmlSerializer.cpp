#include "ExitRespXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ExitResp& input)
{
    auto root = std::make_unique<TreeNode>("ExitResp");
    root->attributes_.insert({"status", std::to_string(input.status)});
    return root;
}
Network::IMessageData Serialize(const ExitResp& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
