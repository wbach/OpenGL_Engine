#pragma once
#include "PlantBrushEnabled.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(PlantBrushEnabled&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializePlantBrushEnabled(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
