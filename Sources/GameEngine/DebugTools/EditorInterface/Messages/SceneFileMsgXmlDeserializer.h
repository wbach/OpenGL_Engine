#pragma once
#include "SceneFileMsg.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SceneFileMsg&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeSceneFileMsg(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
