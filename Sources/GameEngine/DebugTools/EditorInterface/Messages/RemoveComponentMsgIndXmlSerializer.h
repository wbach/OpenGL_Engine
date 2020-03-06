#pragma once
#include <Utils/XML/XmlWriter.h>
#include "RemoveComponentMsgInd.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const RemoveComponentMsgInd&);
Network::IMessageData Serialize(const RemoveComponentMsgInd&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
