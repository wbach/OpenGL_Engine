#pragma once
#include <Utils/XML/XmlWriter.h>
#include "AvailableComponentMsgInd.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const AvailableComponentMsgInd&);
Network::IMessageData Serialize(const AvailableComponentMsgInd&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
