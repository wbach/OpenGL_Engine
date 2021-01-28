#pragma once
#include "ComponentDataMessage.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ComponentDataMessage&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeComponentDataMessage(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
