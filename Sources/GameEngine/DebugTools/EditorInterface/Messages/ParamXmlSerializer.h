#pragma once
#include <Utils/XML/XmlWriter.h>
#include "Param.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const Param&);
Network::IMessageData Serialize(const Param&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
