#include "ComponentDataMessageXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>
#include "ParamXmlSerializer.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ComponentDataMessage& input)
{
    auto root = std::make_unique<TreeNode>("ComponentDataMessage");
    root->attributes_.insert({"name", input.name});
    root->attributes_.insert({"gameObjectId", std::to_string(input.gameObjectId)});
    root->addChild(::Convert("params", input.params));
    return root;
}
Network::IMessageData Serialize(const ComponentDataMessage& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
