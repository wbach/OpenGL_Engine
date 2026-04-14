#pragma once
#include <Types.h>

class TreeNode;
namespace GameEngine
{
class File;

namespace GUI
{
class Layer;
struct Theme;
class Text;

namespace ElementWriter
{
void write(std::vector<Layer>&);
void write(const File&, const Layer&);
void write(const File&, const Theme&);
void write(TreeNode&, const Text&);
}  // namespace ElementWriter
}  // namespace GUI
}  // namespace GameEngine
