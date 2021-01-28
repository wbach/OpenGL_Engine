#pragma once
#include "Param.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(Param&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeParam(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
