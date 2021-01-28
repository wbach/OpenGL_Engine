#pragma once
#include <Utils/XML/XmlWriter.h>
#include "ComponentDeleted.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ComponentDeleted&);
Network::IMessageData Serialize(const ComponentDeleted&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
