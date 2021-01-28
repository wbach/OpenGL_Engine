#pragma once
#include <Utils/XML/XmlWriter.h>
#include "Transform.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const Transform&);
Network::IMessageData Serialize(const Transform&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
