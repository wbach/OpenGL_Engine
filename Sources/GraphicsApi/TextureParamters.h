#pragma once

namespace GraphicsApi
{
enum class TextureType
{
    FLOAT_BUFFER_2D,
    DEPTH_BUFFER_2D,
    FLOAT_TEXTURE_1D,
    FLOAT_TEXTURE_2D,
    FLOAT_TEXTURE_3D,
    FLOAT_TEXTURE_4D,
    SHADOW_MAP,
    U8_RGBA
};

enum class TextureFilter
{
    LINEAR,
    NEAREST
};

enum class TextureMipmap
{
    NONE,
    LINEAR
};

enum class TextureAccess
{
    WRITE_ONLY
};
enum class TextureFormat
{
    BMP,
    JPG,
    PNG
};
}
