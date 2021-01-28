#pragma once
#include <Utils/XML/XmlWriter.h>
#include "LightDeleted.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const LightDeleted&);
Network::IMessageData Serialize(const LightDeleted&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
