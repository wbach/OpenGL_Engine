#include "CameraMsgXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const CameraMsg& input)
{
    auto root = std::make_unique<TreeNode>("CameraMsg");
    root->addChild(::Convert("position", input.position));
    root->addChild(::Convert("rotation", input.rotation));
    return root;
}
Network::IMessageData Serialize(const CameraMsg& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
