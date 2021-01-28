#pragma once
#include <Utils/XML/XmlWriter.h>
#include "PlantBrushEnabled.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const PlantBrushEnabled&);
Network::IMessageData Serialize(const PlantBrushEnabled&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
