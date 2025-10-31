#pragma once
#include "Utils/TreeNode.h"
#include <filesystem>

namespace Utils
{
class XmlReader
{
public:
    bool Read(const std::filesystem::path&);
    bool ReadXml(std::string fileContent);
    TreeNode* Get();
    TreeNode* Get(const std::string& name, TreeNode* node = nullptr);

private:
    std::unique_ptr<TreeNode> root_;
};
}  // namespace Utils
