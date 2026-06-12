#pragma once

#include <cstdint>
#include <vector>

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

struct PipelineConfig
{
    GraphicsApi::ShadersFiles shaders;
    Topology topology                          = Topology::TriangleList;
    PolygonMode polygonMode                    = PolygonMode::Fill;
    CullMode cullMode                          = CullMode::Back;
    bool depthTestEnable                       = true;
    bool depthWriteEnable                      = true;
    bool blendEnable                           = false;
    bool primitiveRestartEnable                = false;
    std::vector<uint32_t> descriptorSetLayouts = {};
    uint32_t pushConstantSize                  = 0;
};

}  // namespace GraphicsApi::Vulkan