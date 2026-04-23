#pragma once
#include <algorithm>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Property.h"
#include "Rotation.h"
#include "Types.h"

typedef std::unordered_map<std::string, std::string> Attributes;

inline const std::string CSTR_X    = "x";
inline const std::string CSTR_Y    = "y";
inline const std::string CSTR_Z    = "z";
inline const std::string CSTR_W    = "w";
inline const std::string CSTR_VEC3 = "vec3";

class ENGINE_API TreeNode
{
public:
    TreeNode();
    TreeNode(const std::string&);

    template <typename T>
    requires std::same_as<std::remove_cvref_t<T>, std::string> TreeNode(const std::string& name, T&& value)
        : parent(nullptr)
        , value_(std::forward<T>(value))
        , type_{"unknown"}
        , name_(name)
    {
    }

    explicit TreeNode(const std::string& name, const std::filesystem::path& path);
    TreeNode(const TreeNode&);
    TreeNode& operator=(const TreeNode&);

    const std::string& name() const;
    const std::vector<std::unique_ptr<TreeNode>>& getChildren() const;
    TreeNode& addChild(const std::string&);
    TreeNode& addChild(const std::string&, const std::string&);
    TreeNode& addChild(const std::string&, const std::filesystem::path&);
    TreeNode& addChild(const std::string&, const std::string_view&);
    TreeNode& addChild(std::unique_ptr<TreeNode>);
    TreeNode& addChild(const TreeNode&);
    TreeNode* getChild(const std::string&) const;
    std::vector<const TreeNode*> getChildren(const std::string&) const;
    bool isAttributePresent(const std::string&) const;
    std::string getAttributeValue(const std::string&) const;

public:
    TreeNode* parent;
    std::string value_;
    std::string type_;
    Attributes attributes_;

private:
    std::vector<std::unique_ptr<TreeNode>> children_;
    std::string name_;

private:
    void CopyTreeNode(const TreeNode&, TreeNode&);
};

std::ostream& operator<<(std::ostream&, const TreeNode&);

vec2 ConvertToVec2(TreeNode&);
vec3 ConvertToVec3(TreeNode&);
std::unique_ptr<TreeNode> Convert(const std::string&);
std::unique_ptr<TreeNode> Convert(const std::string&, const vec2&);
std::unique_ptr<TreeNode> Convert(const std::string&, const vec3&);
std::unique_ptr<TreeNode> Convert(const std::string&, const Quaternion&);

template <class T>
std::unique_ptr<TreeNode> Convert(T v)
{
    return std::make_unique<TreeNode>("v", std::to_string(v));
}

template <class T>
std::unique_ptr<TreeNode> Convert(const std::string& label, const std::vector<T>& v)
{
    auto root = std::make_unique<TreeNode>(label);
    for (const auto& element : v)
    {
        root->addChild(std::move(Convert(element)));
    }
    return root;
}

ENGINE_API void PrintTree(const TreeNode&);
