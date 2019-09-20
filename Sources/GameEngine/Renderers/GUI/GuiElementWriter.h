#pragma once
#include <Types.h>
#include "GameEngine/Renderers/GUI/Layer/GuiLayer.h"

namespace GameEngine
{
namespace GuiElementWriter
{
void write(std::vector<GuiLayer>);
void write(const std::string&, const GuiLayer& layer);
}  // namespace GuiElementWriter
}  // namespace GameEngine
