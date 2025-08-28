#pragma once
#include "Utils/TreeNode.h"

namespace Utils
{
namespace Json
{
void Write(const std::string& filename, const TreeNode& root);
std::string Write(const TreeNode& root);
}  // namespace Json
}  // namespace Utils
