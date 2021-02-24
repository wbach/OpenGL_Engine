#pragma once
#include "BufferParamters.h"
#include "TextureParamters.h"
#include "Types.h"

namespace GraphicsApi
{
struct TextureInfo
{
    uint32 id{0};
    vec2ui size{0, 0};
    TextureType textureType{TextureType::U8_RGBA};
    TextureFilter textureFilter{TextureFilter::LINEAR};
    TextureMipmap textureMipmap{TextureMipmap::NONE};
};
}  // namespace GraphicsApi
