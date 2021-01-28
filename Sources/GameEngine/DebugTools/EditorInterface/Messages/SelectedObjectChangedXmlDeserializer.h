#pragma once
#include "SelectedObjectChanged.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SelectedObjectChanged&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeSelectedObjectChanged(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
