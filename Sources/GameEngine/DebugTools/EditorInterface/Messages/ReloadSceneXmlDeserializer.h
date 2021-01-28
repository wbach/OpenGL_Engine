#pragma once
#include "ReloadScene.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ReloadScene&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeReloadScene(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
