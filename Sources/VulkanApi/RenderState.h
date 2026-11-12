#pragma once
#include <map>
#include <Types.h>

namespace GraphicsApi::Vulkan
{
struct RenderState
{
    uint32 activeProgramId   = 0u;
    uint32 activeTextureUnit = 0u;
    std::map<uint32, uint32> boundShaderBuffers;
    std::map<uint32, uint32> boundTextures;
};
}  // namespace GraphicsApi::Vulkan
