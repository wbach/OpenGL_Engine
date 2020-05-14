#pragma once
#include <Types.h>

namespace GraphicsApi
{
namespace FrameBuffer
{
enum class Type
{
    Color0,
    Color1,
    Color2,
    Color3,
    Depth
};
enum class BindType
{
    ReadWrite,
    Write,
    Read,
    Textures
};

enum class Format
{
    Rgba8,
    Rgba16f,
    Rgba32f,
    Depth
};
struct Attachment
{
    Type type;
    Format format;
    vec2ui size;
    vec4 defaultValue;

    Attachment()
        : Attachment(vec2ui(640, 480), Type::Color0, Format::Rgba8, vec4(0))
    {
    }
    Attachment(const vec2ui& size, Type type, Format format)
        : Attachment(size, type, format, type == Type::Depth ? vec4(1.f) : vec4(0.f))
    {
    }
    Attachment(const vec2ui& size, Type type, Format format, const vec4& defaultValue)
        : type{type}
        , format{format}
        , size{size}
        , defaultValue{defaultValue}
    {
    }
};
}  // namespace FrameBuffer
}  // namespace GraphicsApi
