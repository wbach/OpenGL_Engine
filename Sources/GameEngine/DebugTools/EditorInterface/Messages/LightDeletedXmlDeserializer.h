#pragma once
#include "LightDeleted.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(LightDeleted&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeLightDeleted(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
