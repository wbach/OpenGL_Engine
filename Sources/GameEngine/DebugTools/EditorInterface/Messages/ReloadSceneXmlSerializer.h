#pragma once
#include <Utils/XML/XmlWriter.h>
#include "ReloadScene.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ReloadScene&);
Network::IMessageData Serialize(const ReloadScene&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
