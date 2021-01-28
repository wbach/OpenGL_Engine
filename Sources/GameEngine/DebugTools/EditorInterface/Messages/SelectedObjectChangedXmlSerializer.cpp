#include "SelectedObjectChangedXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const SelectedObjectChanged& input)
{
    auto root = std::make_unique<TreeNode>("SelectedObjectChanged");
    root->attributes_.insert({"id", std::to_string(input.id)});
    return root;
}
Network::IMessageData Serialize(const SelectedObjectChanged& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
