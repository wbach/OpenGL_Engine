#pragma once
#include "TerrainPainterEnabled.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(TerrainPainterEnabled&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeTerrainPainterEnabled(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
