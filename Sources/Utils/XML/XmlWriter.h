#pragma once
#include "Utils/TreeNode.h"
#include "EngineApi.h"

namespace Utils
{
namespace Xml
{
ENGINE_API void Write(const std::filesystem::path&, const TreeNode&);
ENGINE_API std::string Write(const TreeNode&);
}
}  // namespace Utils
