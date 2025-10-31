#pragma once
#include <filesystem>
#include "Utils/TreeNode.h"

namespace Utils
{
namespace Json
{
void Write(const std::filesystem::path& filename, const TreeNode& root);
std::string Write(const TreeNode& root);
}  // namespace Json
}  // namespace Utils
