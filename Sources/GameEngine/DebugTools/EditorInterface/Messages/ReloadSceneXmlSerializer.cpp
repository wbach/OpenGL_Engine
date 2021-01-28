#include "ReloadSceneXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ReloadScene& input)
{
    auto root = std::make_unique<TreeNode>("ReloadScene");
    root->attributes_.insert({"filename", input.filename});
    return root;
}
Network::IMessageData Serialize(const ReloadScene& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
