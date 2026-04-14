#include "TreeNodeWriteFunctions.h"

#include "Utils.h"

void write(TreeNode& node, float v)
{
    node.value_ = std::to_string(v);
}
void write(TreeNode& node, int v)
{
    node.value_ = std::to_string(v);
}
void write(TreeNode& node, uint32 v)
{
    node.value_ = std::to_string(v);
}
void write(TreeNode& node, bool b)
{
    node.value_ = Utils::BoolToString(b);
}
void write(TreeNode& node, const std::string& str)
{
    node.value_ = str;
}
void write(TreeNode& node, const std::string_view& str)
{
    node.value_ = str;
}
void write(TreeNode& node, const std::filesystem::path& value)
{
    node.value_ = value.generic_string();
}
void write(TreeNode& node, const vec2ui& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
}
void write(TreeNode& node, const vec2& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
}
void write(TreeNode& node, const vec3& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
}
void write(TreeNode& node, const vec4& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
    node.attributes_[CSTR_W] = std::to_string(v.w);
}
void write(TreeNode& node, const Color& v)
{
    write(node, v.value);
}
void write(TreeNode& node, const std::vector<vec3>& v)
{
    for (const auto& value : v)
    {
        ::write(node.addChild(CSTR_VEC3), value);
    }
}