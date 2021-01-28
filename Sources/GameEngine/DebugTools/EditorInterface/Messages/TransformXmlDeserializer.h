#pragma once
#include "Transform.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(Transform&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeTransform(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
