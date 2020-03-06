#pragma once
#include <Utils/XML/XmlWriter.h>
#include "NewGameObjectInd.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const NewGameObjectInd&);
Network::IMessageData Serialize(const NewGameObjectInd&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
