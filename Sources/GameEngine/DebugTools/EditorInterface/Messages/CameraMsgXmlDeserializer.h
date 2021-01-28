#pragma once
#include "CameraMsg.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(CameraMsg&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeCameraMsg(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
