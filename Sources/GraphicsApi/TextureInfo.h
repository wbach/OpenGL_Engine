#pragma once
#include "Types.h"
#include "TextureParamters.h"

namespace GraphicsApi
{
struct TextureInfo
{
    uint32 id;
    vec2ui size;
    TextureType textureType;
    TextureFilter textureFilter;
    TextureMipmap textureMipmap;
};
}  // namespace GraphicsApi
