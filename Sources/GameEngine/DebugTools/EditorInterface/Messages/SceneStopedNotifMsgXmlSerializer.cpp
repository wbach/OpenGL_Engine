#include "SceneStopedNotifMsgXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const SceneStopedNotifMsg& input)
{
    auto root = std::make_unique<TreeNode>("SceneStopedNotifMsg");
    root->attributes_.insert({"name", input.name});
    return root;
}
Network::IMessageData Serialize(const SceneStopedNotifMsg& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
