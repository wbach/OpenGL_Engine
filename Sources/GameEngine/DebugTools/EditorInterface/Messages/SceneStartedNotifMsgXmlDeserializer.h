#pragma once
#include "SceneStartedNotifMsg.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SceneStartedNotifMsg&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeSceneStartedNotifMsg(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
