#pragma once
#include <Utils/XML/XmlWriter.h>
#include "SelectedObjectChanged.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<TreeNode> Convert(const SelectedObjectChanged&);
Network::IMessageData Serialize(const SelectedObjectChanged&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
