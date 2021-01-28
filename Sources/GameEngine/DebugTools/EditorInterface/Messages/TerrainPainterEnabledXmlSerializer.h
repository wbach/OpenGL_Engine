#pragma once
#include <Utils/XML/XmlWriter.h>
#include "TerrainPainterEnabled.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const TerrainPainterEnabled&);
Network::IMessageData Serialize(const TerrainPainterEnabled&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
