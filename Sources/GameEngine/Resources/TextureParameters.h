#pragma once
#include <Types.h>

#include "DataStorePolicy.h"
#include "Textures/TextureFlip.h"
#include <GraphicsApi/TextureParamters.h>

namespace GameEngine
{
enum class TextureLoadType
{
    AddToGpuPass,
    Immediately,
    None
};
enum class SizeLimitPolicy
{
    Limited,
    NoLimited
};

struct TextureParameters
{
    TextureFlip flipMode{TextureFlip::NONE};
    TextureLoadType loadType{TextureLoadType::AddToGpuPass};
    SizeLimitPolicy sizeLimitPolicy{SizeLimitPolicy::Limited};
    DataStorePolicy dataStorePolicy{DataStorePolicy::ToRelease};
    GraphicsApi::TextureFilter filter{GraphicsApi::TextureFilter::NEAREST};
    GraphicsApi::TextureMipmap mimap{GraphicsApi::TextureMipmap::NONE};
};
typedef vec2ui TextureSize;
typedef void* RawData;
}  // namespace GameEngine
