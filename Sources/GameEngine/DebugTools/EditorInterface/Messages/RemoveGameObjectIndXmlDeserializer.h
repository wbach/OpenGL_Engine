#pragma once
#include "RemoveGameObjectInd.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(RemoveGameObjectInd&, Utils::XmlNode&);
std::unique_ptr<Network::IMessage> DeserializeRemoveGameObjectInd(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
