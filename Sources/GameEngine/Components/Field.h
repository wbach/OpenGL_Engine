#pragma once
#include <Types.h>

#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
enum class FieldType
{
    Int = 0,
    UInt,
    Float,
    String,
    Boolean,
    Texture,
    File,
    AnimationClip,
    Material,
    Vector2i,
    Vector2f,
    Vector3f,
    Vector4f,
    ColorRGB,
    ColorRGBA,
    Enum,
    VectorOfStrings,
    VectorOfInt,
    VectorOfFloat,
    VectorOfFiles,
    VectorOfTextures,
    VectorOfVector3f,
    VectorOfTerrainTextures,
    VectorOfAnimationClips,
    ConstVectorOfTextures,
    ConstMapOfMaterials,
    Custom,
    VectorOfCustom
};

struct ENGINE_API FieldInfo
{
    std::string name{""};
    FieldType type{FieldType::Int};
    void* ptr{nullptr};

    std::function<std::vector<std::string>()> enumNames = {};
    std::function<int(void*)> enumToIndex               = {};
    std::function<void(void*, int)> indexToEnum         = {};
};

inline std::ostream& operator<<(std::ostream& os, const FieldInfo& info)
{
    os << "FieldInfo(\n"
       << "Name: " << info.name << ", \n"
       << "Type: " << magic_enum::enum_name(info.type) << ",\n"
       << "Ptr: [" << info.ptr << "])";

    return os;
}
}  // namespace Components
}  // namespace GameEngine
