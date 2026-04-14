#include "ComponentDeletedXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ComponentDeleted& input)
{
    auto root = std::make_unique<TreeNode>("ComponentDeleted");
    root->attributes_.insert({"gameObjectId", std::to_string(input.gameObjectId)});
    root->attributes_.insert({"componentName", input.componentName});
    return root;
}
Network::IMessageData Serialize(const ComponentDeleted& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
