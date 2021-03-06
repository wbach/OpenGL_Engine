#include "NewGameObjectIndXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const NewGameObjectInd& input)
{
    auto root = std::make_unique<TreeNode>("NewGameObjectInd");
    root->attributes_.insert({"id", std::to_string(input.id)});
    root->attributes_.insert({"parentId", std::to_string(input.parentId)});
    root->attributes_.insert({"name", input.name});
    return root;
}
Network::IMessageData Serialize(const NewGameObjectInd& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
