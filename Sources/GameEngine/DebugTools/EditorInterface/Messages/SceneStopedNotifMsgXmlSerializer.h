#pragma once
#include <Utils/XML/XmlWriter.h>
#include "SceneStopedNotifMsg.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const SceneStopedNotifMsg&);
Network::IMessageData Serialize(const SceneStopedNotifMsg&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
