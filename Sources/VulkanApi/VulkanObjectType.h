#pragma once

namespace GraphicsApi::Vulkan
{
enum class VulkanObjectType
{
    TEXTURE_2D,
    TEXTURE_2D_ARRAY,
    TEXTURE_CUBE_MAP,
    FRAME_BUFFER,
    RENDER_BUFFER,
    SHADER_PROGRAM,
    SHADER_BUFFER,
    MESH
};
}
