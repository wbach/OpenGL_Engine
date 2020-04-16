#pragma once
#include "SelectedObjectChanged.h"
#include <Utils/XML/XmlReader.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
void SetParam(SelectedObjectChanged&, Utils::XmlNode&);
std::unique_ptr<Network::IMessage> DeserializeSelectedObjectChanged(Utils::XmlReader& reader);
} // namespace DebugNetworkInterface
} // namespace GameEngine
