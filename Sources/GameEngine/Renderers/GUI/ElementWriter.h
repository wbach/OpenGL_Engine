#pragma once
#include <Types.h>

namespace GameEngine
{
class File;

namespace GUI
{
class Layer;
struct Theme;

namespace ElementWriter
{
void write(std::vector<Layer>&);
void write(const File&, const Layer&);
void write(const File&, const Theme&);
}  // namespace ElementWriter
}  // namespace GUI
}  // namespace GameEngine
