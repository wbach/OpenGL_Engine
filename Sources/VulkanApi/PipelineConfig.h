#pragma once
#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/ShaderFiles.h"

namespace GraphicsApi::Vulkan
{

enum class Topology
{
    TriangleList,
    TriangleStrip,
    PointList,
    LineList,
    PatchList
};

enum class PolygonMode
{
    Fill,
    Line,
    Point
};

enum class CullMode
{
    None,
    Front,
    Back,
    FrontAndBack
};

struct VertexAttribute
{
    uint32_t location{0};
    VkFormat format{VK_FORMAT_UNDEFINED};
    uint32_t offset{0};
};

struct VertexLayout
{
    uint32_t stride{0};
    std::vector<VertexAttribute> attributes;
};

struct DescriptorBindingConfig
{
    uint32 binding = 0;

    VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    uint32 descriptorCount = 1;

    VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
};

VertexLayout GetDefaultVertexLayout();
std::vector<DescriptorBindingConfig> GetDefaultDescriptorBindings();
GraphicsApi::DrawFlag GetDrawFlagByBindLocation(uint32);

struct PipelineConfig
{
    GraphicsApi::ShadersFiles shaders;
    Topology topology                                       = Topology::TriangleList;
    PolygonMode polygonMode                                 = PolygonMode::Fill;
    CullMode cullMode                                       = CullMode::Back;
    bool depthTestEnable                                    = true;
    bool depthWriteEnable                                   = true;
    bool blendEnable                                        = false;
    bool primitiveRestartEnable                             = false;
    VertexLayout vertexLayout                               = GetDefaultVertexLayout();
    std::vector<DescriptorBindingConfig> descriptorBindings = GetDefaultDescriptorBindings();
};

}  // namespace GraphicsApi::Vulkan