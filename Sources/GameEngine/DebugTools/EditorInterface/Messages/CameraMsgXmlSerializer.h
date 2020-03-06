#pragma once
#include <Utils/XML/XmlWriter.h>
#include "CameraMsg.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const CameraMsg&);
Network::IMessageData Serialize(const CameraMsg&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
