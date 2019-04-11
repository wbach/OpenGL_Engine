#pragma once
#include "BufferParamters.h"
#include "TextureParamters.h"
#include "Types.h"

namespace GraphicsApi
{
struct TextureInfo
{
    uint32 id;
    vec2ui size;
    TextureType textureType;
    TextureFilter textureFilter;
    TextureMipmap textureMipmap;
    BufferAtachment atachment;
};
}  // namespace GraphicsApi
