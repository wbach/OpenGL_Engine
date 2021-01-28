#pragma once
#include "NewLight.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewLight&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeNewLight(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
