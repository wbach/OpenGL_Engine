#pragma once
#include "ComponentDeleted.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ComponentDeleted&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeComponentDeleted(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
