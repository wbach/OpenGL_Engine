#pragma once
#include <memory>
#include <string>
#include <filesystem>

#include "TreeNode.h"

namespace Utils
{
class JsonReader
{
public:
    bool Read(const std::filesystem::path&);
    bool ReadJson(const std::string&);
    TreeNode* Get();
    TreeNode* Get(const std::string& name, TreeNode* node = nullptr);

private:
    std::unique_ptr<TreeNode> root_;
};
}  // namespace Utils
