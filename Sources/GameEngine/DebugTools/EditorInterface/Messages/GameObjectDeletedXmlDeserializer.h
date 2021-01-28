#pragma once
#include "GameObjectDeleted.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(GameObjectDeleted&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeGameObjectDeleted(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
