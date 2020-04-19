#pragma once
#include "SceneStopedNotifMsg.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SceneStopedNotifMsg&, Utils::XmlNode&);
std::unique_ptr<Network::IMessage> DeserializeSceneStopedNotifMsg(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
