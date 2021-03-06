#include "PlantBrushEnabledXmlSerializer.h"
#include <Utils.h>
#include <Utils/TreeNode.h>
#include <UtilsNetwork/MessageFormat.h>
#include <UtilsNetwork/Messages/XmlConverterUtils.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const PlantBrushEnabled& input)
{
    auto root = std::make_unique<TreeNode>("PlantBrushEnabled");
    root->addChild(::Convert("brushTypes", input.brushTypes));
    root->attributes_.insert({"texture", input.texture});
    root->attributes_.insert({"brushSize", std::to_string(input.brushSize)});
    return root;
}
Network::IMessageData Serialize(const PlantBrushEnabled& input)
{
    auto root = Convert(input);
    return Network::CreatePayload(*root);
}
} // namespace DebugNetworkInterface
} // namespace GameEngine
