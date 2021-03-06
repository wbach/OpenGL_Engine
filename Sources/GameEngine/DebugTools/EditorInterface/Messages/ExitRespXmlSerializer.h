#pragma once
#include <Utils/XML/XmlWriter.h>
#include "ExitResp.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const ExitResp&);
Network::IMessageData Serialize(const ExitResp&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
