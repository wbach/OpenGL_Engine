#pragma once
#include "NewComponentMsgInd.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewComponentMsgInd&, Utils::XmlNode&);
std::unique_ptr<Network::IMessage> DeserializeNewComponentMsgInd(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
