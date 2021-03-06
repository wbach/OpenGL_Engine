#pragma once
#include "RemoveComponentMsgInd.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(RemoveComponentMsgInd&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeRemoveComponentMsgInd(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
