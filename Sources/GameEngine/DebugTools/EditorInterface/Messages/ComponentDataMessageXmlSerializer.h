#pragma once
#include <Utils/XML/XmlWriter.h>
#include "ComponentDataMessage.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ComponentDataMessage&);
Network::IMessageData Serialize(const ComponentDataMessage&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
