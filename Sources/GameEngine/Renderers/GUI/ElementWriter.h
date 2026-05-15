#pragma once
#include <Types.h>

#include <vector>

#include "Element.h"

class TreeNode;
namespace GameEngine
{
class File;

namespace GUI
{
class Text;
struct Theme;
class Button;
class Sprite;

class MultiLineText;
namespace ElementWriter
{
void write(const File&, const Element&);
void write(const File&, const Element::Children&);
void write(const File&, const Theme&);
void write(TreeNode&, const Text&);
void write(TreeNode&, const MultiLineText&);
void write(TreeNode&, const Button&);
void write(TreeNode&, const Sprite&);
}  // namespace ElementWriter
}  // namespace GUI
}  // namespace GameEngine
