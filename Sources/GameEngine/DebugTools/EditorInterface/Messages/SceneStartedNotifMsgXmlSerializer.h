#pragma once
#include <Utils/XML/XmlWriter.h>
#include "SceneStartedNotifMsg.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const SceneStartedNotifMsg&);
Network::IMessageData Serialize(const SceneStartedNotifMsg&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
