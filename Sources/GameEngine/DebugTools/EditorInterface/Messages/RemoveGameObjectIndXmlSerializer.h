#pragma once
#include <Utils/XML/XmlWriter.h>
#include "RemoveGameObjectInd.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const RemoveGameObjectInd&);
Network::IMessageData Serialize(const RemoveGameObjectInd&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
