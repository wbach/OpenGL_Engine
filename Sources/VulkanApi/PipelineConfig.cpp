#include "PipelineConfig.h"

namespace GraphicsApi::Vulkan
{
namespace
{
const uint32 PER_APP_BIND_LOCATION              = 0;
const uint32 PER_FRAME_BIND_LOCATION            = 1;
const uint32 PER_OBJECT_CONSTANTS_BIND_LOCATION = 2;
const uint32 PER_OBJECT_UPDATE_BIND_LOCATION    = 3;  // PER_TERRAIN_BIND_LOCATION
const uint32 PER_POSE_UPDATE_BIND_LOCATION      = 4;  // PER_NODE_LOCATION
const uint32 PER_INSTANCES_BIND_LOCATION        = 5;
const uint32 PER_MESH_OBJECT_BIND_LOCATION      = 6;
const uint32 SHADOW_BUFFER_BIND_LOCATION        = 7;
const uint32 WATER_TILE_BIND_LOCATION           = 8;
const uint32 DIRECTIONAL_LIGHTS_BIND_LOCATION   = 9;
const uint32 POINT_LIGHTS_BIND_LOCATION         = 10;
const uint32 SPOT_LIGHTS_BIND_LOCATION          = 11;
}  // namespace

std::vector<DescriptorBindingConfig> GetDefaultDescriptorBindings()
{
    constexpr auto globalStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    return {// 0 - PER_APP
            {.binding         = PER_APP_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 1 - PER_FRAME
            {.binding         = PER_FRAME_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 2 - PER_OBJECT_CONSTANTS
            {.binding         = PER_OBJECT_CONSTANTS_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 3 - PER_OBJECT_UPDATE
            {.binding         = PER_OBJECT_UPDATE_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 4 - PER_POSE_UPDATE
            {.binding         = PER_POSE_UPDATE_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 5 - PER_INSTANCES
            {.binding         = PER_INSTANCES_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 6 - PER_MESH_OBJECT
            {.binding         = PER_MESH_OBJECT_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 7 - SHADOW
            {.binding         = SHADOW_BUFFER_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 8 - WATER_TILE
            {.binding         = WATER_TILE_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 9 - DIRECTIONAL_LIGHTS
            {.binding         = DIRECTIONAL_LIGHTS_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 10 - POINT_LIGHTS
            {.binding         = POINT_LIGHTS_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages},

            // 11 - SPOT_LIGHTS
            {.binding         = SPOT_LIGHTS_BIND_LOCATION,
             .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
             .descriptorCount = 1,
             .stageFlags      = globalStages}};
}
VertexLayout GetDefaultVertexLayout()
{
    return {sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 4 + sizeof(int) * 4,
            {
                {0, VK_FORMAT_R32G32B32_SFLOAT, 0},                      // Position
                {1, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 3},         // TexCoord
                {2, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 5},      // Normal
                {3, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 8},      // Tangent
                {4, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 11},  // Weights
                {5, VK_FORMAT_R32G32B32A32_SINT, sizeof(float) * 15}     // BoneIds
            }};
}
GraphicsApi::DrawFlag GetDrawFlagByBindLocation(uint32 bindLocation)
{
    using namespace GraphicsApi::Vulkan;

    switch (bindLocation)
    {
        case PER_FRAME_BIND_LOCATION:           // 1
                                                //    case PER_OBJECT_UPDATE_BIND_LOCATION:   // 3
        case PER_POSE_UPDATE_BIND_LOCATION:     // 4
        case PER_INSTANCES_BIND_LOCATION:       // 5
        case SHADOW_BUFFER_BIND_LOCATION:       // 7
        case DIRECTIONAL_LIGHTS_BIND_LOCATION:  // 9
        case POINT_LIGHTS_BIND_LOCATION:        // 10
        case SPOT_LIGHTS_BIND_LOCATION:         // 11
            return GraphicsApi::DrawFlag::Dynamic;

        default:
            return GraphicsApi::DrawFlag::Static;
    }
}
}  // namespace GraphicsApi::Vulkan