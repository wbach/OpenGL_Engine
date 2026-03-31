#pragma once
#include <Types.h>

namespace GameEngine
{
class File;
class GuiLayer;
namespace GuiElementWriter
{
void write(std::vector<GuiLayer>);
void write(const File&, const GuiLayer&);
}  // namespace GuiElementWriter
}  // namespace GameEngine
