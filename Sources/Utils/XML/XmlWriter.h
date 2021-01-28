#pragma once
#include "Utils/TreeNode.h"

namespace Utils
{
namespace Xml
{
void Write(const std::string&, TreeNode&);
std::string Write(TreeNode&);
}
}  // namespace Utils
