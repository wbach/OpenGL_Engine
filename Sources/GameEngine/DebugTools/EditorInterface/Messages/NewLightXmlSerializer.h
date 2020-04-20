#pragma once
#include <Utils/XML/XmlWriter.h>
#include "NewLight.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const NewLight&);
Network::IMessageData Serialize(const NewLight&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
