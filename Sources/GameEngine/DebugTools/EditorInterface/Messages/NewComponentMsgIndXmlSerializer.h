#pragma once
#include <Utils/XML/XmlWriter.h>
#include "NewComponentMsgInd.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const NewComponentMsgInd&);
Network::IMessageData Serialize(const NewComponentMsgInd&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
