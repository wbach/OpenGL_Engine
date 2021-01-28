#pragma once
#include "GameObjectRenamed.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(GameObjectRenamed&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeGameObjectRenamed(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
