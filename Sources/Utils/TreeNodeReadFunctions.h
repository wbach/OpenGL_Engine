#pragma once
#include "TreeNode.h"

ENGINE_API void Read(const TreeNode&, float&);
ENGINE_API void Read(const TreeNode&, bool&);
ENGINE_API void Read(const TreeNode&, int32&);
ENGINE_API void Read(const TreeNode&, uint8&);
ENGINE_API void Read(const TreeNode&, uint32&);
ENGINE_API void Read(const TreeNode&, uint64&);
ENGINE_API void Read(const TreeNode&, vec2&);
ENGINE_API void Read(const TreeNode&, vec2ui&);
ENGINE_API void Read(const TreeNode&, vec3&);
ENGINE_API void Read(const TreeNode&, vec4&);
ENGINE_API void Read(const TreeNode&, Color&);
ENGINE_API void Read(const TreeNode&, Quaternion&);
ENGINE_API void Read(const TreeNode&, Rotation&);
ENGINE_API void Read(const TreeNode&, std::string&);
ENGINE_API void Read(const TreeNode&, std::filesystem::path&);
ENGINE_API void Read(const TreeNode&, std::vector<vec2>&);
ENGINE_API void Read(const TreeNode&, std::vector<vec3>&);
ENGINE_API void Read(const TreeNode&, std::vector<std::string>&);

template <class T>
void Read(const TreeNode& node, std::optional<T>& v)
{
    v.emplace();
    Read(node, *v);
}

template <class T>
void Read(const TreeNode& node, Property<T>& v)
{
    Read(node, v.modify());
}

template <class T>
requires std::is_enum_v<T>
void Read(const TreeNode& node, T& v)
{
    if (auto enum_value = magic_enum::enum_cast<T>(node.value_))
    {
        v = *enum_value;
    }
}

template <class T>
requires std::is_enum_v<T>
void Read(const TreeNode* node, T& v)
{
    if (node)
    {
        if (auto enum_value = magic_enum::enum_cast<T>(node->value_))
        {
            v = *enum_value;
        }
    }
}

template <class T>
requires(!std::is_enum_v<T>) void Read(const TreeNode* node, T& v)
{
    if (node)
    {
        Read(*node, v);
    }
}