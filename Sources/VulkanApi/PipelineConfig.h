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

enum class FrontFace
{
    Clockwise,
    CounterClockwise
};

enum class CompareOp
{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

struct ColorBlendConfig
{
    bool blendEnable = false;
};

struct InputAssemblyConfig
{
    Topology topology           = Topology::TriangleList;
    bool primitiveRestartEnable = false;
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

struct RasterizationConfig
{
    PolygonMode polygonMode = PolygonMode::Fill;
    CullMode cullMode = CullMode::Back;

    FrontFace frontFace = FrontFace::CounterClockwise;

    bool depthClampEnable = false;
    bool rasterizerDiscardEnable = false;

    bool depthBiasEnable = false;

    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;

    float lineWidth = 1.0f;
};

struct DepthStencilConfig
{
    bool depthTestEnable  = true;
    bool depthWriteEnable = true;

    CompareOp depthCompareOp = CompareOp::Less;

    bool depthBoundsTestEnable = false;

    bool stencilTestEnable = false;
};

VertexLayout GetDefaultVertexLayout();
std::vector<DescriptorBindingConfig> GetDefaultDescriptorBindings();
GraphicsApi::DrawFlag GetDrawFlagByBindLocation(uint32);

struct PipelineConfig
{
    GraphicsApi::ShadersFiles shaders;

    InputAssemblyConfig inputAssembly;
    RasterizationConfig rasterization;
    DepthStencilConfig depthStencil;
    ColorBlendConfig colorBlend;

    VertexLayout vertexLayout = GetDefaultVertexLayout();
    std::vector<DescriptorBindingConfig> descriptorBindings = GetDefaultDescriptorBindings();
};

}  // namespace GraphicsApi::Vulkan