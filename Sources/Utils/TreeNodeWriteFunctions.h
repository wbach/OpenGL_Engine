#pragma once
#include <filesystem>
#include <magic_enum/magic_enum.hpp>

#include "EngineApi.h"
#include "Property.h"
#include "TreeNode.h"
#include "Types.h"

ENGINE_API void write(TreeNode&, float);
ENGINE_API void write(TreeNode&, int);
ENGINE_API void write(TreeNode&, uint8);
ENGINE_API void write(TreeNode&, uint32);
ENGINE_API void write(TreeNode&, uint64);
ENGINE_API void write(TreeNode&, bool);
ENGINE_API void write(TreeNode&, const std::string&);
ENGINE_API void write(TreeNode&, const std::string_view&);
ENGINE_API void write(TreeNode&, const std::filesystem::path&);
ENGINE_API void write(TreeNode&, const vec2ui&);
ENGINE_API void write(TreeNode&, const vec2&);
ENGINE_API void write(TreeNode&, const vec3&);
ENGINE_API void write(TreeNode&, const vec4&);
ENGINE_API void write(TreeNode&, const Color&);
ENGINE_API void write(TreeNode&, const std::vector<vec3>&);

template <class T>
requires std::is_enum_v<T>
void write(TreeNode& node, const T& v)
{
    write(node, magic_enum::enum_name(v));
}

template <class T>
void write(TreeNode& node, const std::optional<T>& v)
{
    if (v)
    {
        write(node, *v);
    }
}

template <class T>
void write(TreeNode& node, const Property<T>& v)
{
    write(node, v.get());
}

template <class T>
void write(TreeNode& parent, const std::string& name, const T& v)
{
    write(parent.addChild(name), v);
}

template <class T>
requires std::is_enum_v<T>
void write(TreeNode& node, const std::string& name, const T& v)
{
    write(node, name, magic_enum::enum_name(v));
}

template <class T>
void write(TreeNode& parent, const std::string& name, const std::optional<T>& v)
{
    if (v)
    {
        write(parent.addChild(name), v);
    }
}

template <class T>
void write(TreeNode& parent, const std::string& name, const Property<T>& v)
{
    write(parent, name, v.get());
}
