#pragma once
#include "NewGameObjectInd.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(NewGameObjectInd&, TreeNode&);
std::unique_ptr<Network::IMessage> DeserializeNewGameObjectInd(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
