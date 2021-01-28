#pragma once
#include <Utils/XML/XmlWriter.h>
#include "GameObjectDeleted.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const GameObjectDeleted&);
Network::IMessageData Serialize(const GameObjectDeleted&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
