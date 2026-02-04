#pragma once
#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Types.h"

typedef std::unordered_map<std::string, std::string> Attributes;

class TreeNode
{
public:
    TreeNode();
    TreeNode(const std::string&);
    TreeNode(const std::string&, const std::string&);
    TreeNode(const TreeNode&);
    TreeNode& operator=(const TreeNode&);

    const std::string& name() const;
    const std::vector<std::unique_ptr<TreeNode>>& getChildren() const;
    TreeNode& addChild(const std::string&);
    TreeNode& addChild(const std::string&, const std::string&);
    TreeNode& addChild(const std::string&, const std::filesystem::path&);
    TreeNode& addChild(const std::string&, const std::string_view&);
    TreeNode& addChild(std::unique_ptr<TreeNode>);
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

extern const std::string CSTR_VEC3;

void Read(const TreeNode&, float&);
void Read(const TreeNode&, bool&);
void Read(const TreeNode&, int32&);
void Read(const TreeNode&, uint32&);
void Read(const TreeNode&, vec2&);
void Read(const TreeNode&, vec2ui&);
void Read(const TreeNode&, vec3&);
void Read(const TreeNode&, vec4&);
void Read(const TreeNode&, Color&);
void Read(const TreeNode&, Quaternion&);
void Read(const TreeNode&, std::string&);
void Read(const TreeNode&, std::vector<vec2>&);
void Read(const TreeNode&, std::vector<vec3>&);

template <class T>
void Read(const TreeNode* node, T& v)
{
    if (node)
    {
        Read(*node, v);
    }
}

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

void write(TreeNode&, float);
void write(TreeNode&, int);
void write(TreeNode&, uint32);
void write(TreeNode&, bool);
void write(TreeNode&, const std::string&);
void write(TreeNode&, const std::string_view&);
void write(TreeNode&, const std::filesystem::path&);
void write(TreeNode&, const vec2ui&);
void write(TreeNode&, const vec2&);
void write(TreeNode&, const vec3&);
void write(TreeNode&, const vec4&);
void write(TreeNode&, const Color&);
void write(TreeNode&, const std::optional<uint32>&);
void write(TreeNode&, const std::vector<vec3>&);

void PrintTree(const TreeNode&);