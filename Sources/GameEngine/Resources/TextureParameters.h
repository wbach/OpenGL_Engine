#pragma once
#include <Types.h>
#include "Textures/TextureFlip.h"

namespace GameEngine
{
enum class TextureLoadType
{
    AddToGpuPass,
    Immediately,
    None
};

struct TextureParameters
{
    TextureFlip flipMode{TextureFlip::NONE};
    TextureLoadType loadType{TextureLoadType::AddToGpuPass};
    bool applySizeLimit{true};
};
typedef vec2ui TextureSize;
typedef void* RawData;
}  // namespace GameEngine
