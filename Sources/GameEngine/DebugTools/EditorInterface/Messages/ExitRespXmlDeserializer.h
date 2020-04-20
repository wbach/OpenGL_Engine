#pragma once
#include "ExitResp.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(ExitResp&, Utils::XmlNode&);
std::unique_ptr<Network::IMessage> DeserializeExitResp(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
