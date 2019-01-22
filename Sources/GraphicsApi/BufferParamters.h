#pragma once

namespace GraphicsApi
{
enum class BufferType
{
    COLOR,
    DEPTH,
    RENDER
};
enum class BufferAtachment
{
    NONE,
    COLOR_1,
    COLOR_2,
    COLOR_3,
    COLOR_4,
    DEPTH
};
enum class BindType
{
    DEFAULT,
    DRAW
};
}
