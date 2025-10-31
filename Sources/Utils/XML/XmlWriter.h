#pragma once
#include "Utils/TreeNode.h"

namespace Utils
{
namespace Xml
{
void Write(const std::filesystem::path&, const TreeNode&);
std::string Write(const TreeNode&);
}
}  // namespace Utils
