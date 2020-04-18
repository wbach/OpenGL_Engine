#pragma once
#include <Utils/XML/XmlWriter.h>
#include "SceneFileMsg.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
std::unique_ptr<Utils::XmlNode> Convert(const SceneFileMsg&);
Network::IMessageData Serialize(const SceneFileMsg&);
} // namespace DebugNetworkInterface
} // namespace GameEngine
