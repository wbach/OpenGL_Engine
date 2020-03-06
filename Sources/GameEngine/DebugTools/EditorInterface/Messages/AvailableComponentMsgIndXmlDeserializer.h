#pragma once
#include "AvailableComponentMsgInd.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(AvailableComponentMsgInd&, Utils::XmlNode&);
std::unique_ptr<Network::IMessage> DeserializeAvailableComponentMsgInd(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
