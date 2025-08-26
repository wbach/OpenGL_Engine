#pragma once
#include <memory>
#include <string>

#include "TreeNode.h"

namespace Utils
{
class JsonReader
{
public:
    bool Read(const std::string& filename);
    bool ReadJson(const std::string& fileContent);
    TreeNode* Get();
    TreeNode* Get(const std::string& name, TreeNode* node = nullptr);

private:
    std::unique_ptr<TreeNode> root_;
};
}  // namespace Utils
