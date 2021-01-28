#pragma once
#include <Utils/XML/XmlWriter.h>
#include "GameObjectRenamed.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const GameObjectRenamed&);
Network::IMessageData Serialize(const GameObjectRenamed&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
