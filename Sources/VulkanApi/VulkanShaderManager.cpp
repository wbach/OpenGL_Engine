#include "VulkanShaderManager.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <vector>

#include "GraphicsApi/ShaderProgramType.h"
#include "ShaderTypes.h"
#include "SimpleForwardShaderFiles.h"
#include "Types.h"
#include "VulkanApi/PipelineConfig.h"
#include "VulkanShaderCompiler.h"
#include "magic_enum/magic_enum.hpp"

namespace GraphicsApi::Vulkan
{
namespace
{
std::optional<VkBlendFactor> ToVkBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;

        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;

        case BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

        case BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;

        case BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;

        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

        case BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;

        case BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

        case BlendFactor::ConstantColor:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;

        case BlendFactor::OneMinusConstantColor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

        case BlendFactor::ConstantAlpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;

        case BlendFactor::OneMinusConstantAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    }

    return std::nullopt;
}
std::optional<VkBlendOp> ToVkBlendOperation(BlendOperation operation)
{
    switch (operation)
    {
        case BlendOperation::Add:
            return VK_BLEND_OP_ADD;

        case BlendOperation::Subtract:
            return VK_BLEND_OP_SUBTRACT;

        case BlendOperation::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;

        case BlendOperation::Min:
            return VK_BLEND_OP_MIN;

        case BlendOperation::Max:
            return VK_BLEND_OP_MAX;
    }

    return std::nullopt;
}
VkShaderModule CreateShaderModule(VulkanContext& context, const std::vector<uint32_t>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode    = code.data();

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    if (vkCreateShaderModule(context.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create VkShaderModule!\n";
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

bool LoadShaderSourceFiles(const std::filesystem::path& shaderDirectory, const std::string& vertName, const std::string& fragName,
                           std::string& vertCode, std::string& fragCode)
{
    const auto vertPathFullPath = std::filesystem::absolute(shaderDirectory / vertName).lexically_normal();
    const auto fragPathFullPath = std::filesystem::absolute(shaderDirectory / fragName).lexically_normal();

    vertCode = Utils::ReadFilesWithIncludes(vertPathFullPath);
    fragCode = Utils::ReadFilesWithIncludes(fragPathFullPath);

    if (vertCode.empty() or fragCode.empty())
    {
        LOG_ERROR << "Error: Shader source is empty or the files could not be opened.\n";
        return false;
    }

    return true;
}

bool CompileShaderSources(const std::string& vertName, const std::string& fragName, const std::string& vertCode,
                          const std::string& fragCode, std::vector<uint32_t>& vertSpirv, std::vector<uint32_t>& fragSpirv)
{
    vertSpirv = CompileGlslToSpirv(vertName, shaderc_vertex_shader, vertCode);
    fragSpirv = CompileGlslToSpirv(fragName, shaderc_fragment_shader, fragCode);

    if (vertSpirv.empty() or fragSpirv.empty())
    {
        LOG_ERROR << "Error: Shader compilation failed.\n";
        return false;
    }

    return true;
}

bool CreateShaderModules(VulkanContext& context, const std::vector<uint32_t>& vertSpirv, const std::vector<uint32_t>& fragSpirv,
                         VkShaderModule& vertModule, VkShaderModule& fragModule)
{
    vertModule = CreateShaderModule(context, vertSpirv);
    fragModule = CreateShaderModule(context, fragSpirv);

    if (vertModule == VK_NULL_HANDLE or fragModule == VK_NULL_HANDLE)
    {
        LOG_ERROR << "Error: Failed to allocate VkShaderModule.\n";
        return false;
    }

    return true;
}

VkPolygonMode ToVkPolygonMode(PolygonMode mode)
{
    switch (mode)
    {
        case PolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;

        case PolygonMode::Line:
            return VK_POLYGON_MODE_LINE;

        case PolygonMode::Point:
            return VK_POLYGON_MODE_POINT;

        default:
            return VK_POLYGON_MODE_FILL;
    }
}

VkCullModeFlags ToVkCullMode(CullMode mode)
{
    switch (mode)
    {
        case CullMode::None:
            return VK_CULL_MODE_NONE;

        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;

        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;

        case CullMode::FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;

        default:
            return VK_CULL_MODE_BACK_BIT;
    }
}

VkFrontFace ToVkFrontFace(FrontFace frontFace)
{
    switch (frontFace)
    {
        case FrontFace::Clockwise:
            return VK_FRONT_FACE_CLOCKWISE;

        case FrontFace::CounterClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;

        default:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

VkCompareOp ToVkCompareOp(CompareOp op)
{
    switch (op)
    {
        case CompareOp::Never:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::Less:
            return VK_COMPARE_OP_LESS;
        case CompareOp::Equal:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::LessOrEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::Greater:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOp::GreaterOrEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::Always:
            return VK_COMPARE_OP_ALWAYS;
    }

    // Niepoprawna wartość enuma.
    return VK_COMPARE_OP_MAX_ENUM;
}

std::optional<VkPrimitiveTopology> ToVkPrimitiveTopology(Topology topology)
{
    switch (topology)
    {
        case Topology::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        case Topology::TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            // case Topology::PointList:
            //     inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            //     break;
            // case Topology::LineList:
            //     inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            //     break;
            // case Topology::PatchList:
            //     inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            //     break;
        default:
            return std::nullopt;
            //     return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

bool CreatePipelineLayout(VulkanContext& context, VulkanProgram& newProgram)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &newProgram.layout) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Pipeline Layout!\n";
        return false;
    }

    return true;
}

bool CreateDescriptorSetLayouts(VulkanContext& context, VulkanProgram& program)
{
    using BindingMap = std::map<uint32, VkDescriptorSetLayoutBinding>;

    std::map<uint32, BindingMap> bindingsBySet;

    uint32 maxSet    = 0;
    bool hasBindings = false;

    // Group bindings by descriptor set.
    for (const auto& config : program.pipelineConfig.descriptorBindings)
    {
        hasBindings = true;
        maxSet      = std::max(maxSet, config.set);

        auto& setBindings = bindingsBySet[config.set];

        auto it = setBindings.find(config.binding);

        if (it != setBindings.end())
        {
            // Same binding must have the same descriptor type/count.
            if (it->second.descriptorType != config.descriptorType || it->second.descriptorCount != config.descriptorCount)
            {
                LOG_ERROR << "Conflicting descriptor binding: set=" << config.set << ", binding=" << config.binding;

                return false;
            }

            // Merge shader stages.
            it->second.stageFlags |= config.stageFlags;
            continue;
        }

        VkDescriptorSetLayoutBinding binding{};

        binding.binding            = config.binding;
        binding.descriptorType     = config.descriptorType;
        binding.descriptorCount    = config.descriptorCount;
        binding.stageFlags         = config.stageFlags;
        binding.pImmutableSamplers = nullptr;

        setBindings.emplace(config.binding, binding);
    }

    program.descriptorSetLayouts.clear();

    if (!hasBindings)
        return true;

    // Preserve set indices, including empty intermediate sets.
    program.descriptorSetLayouts.resize(static_cast<size_t>(maxSet) + 1, VK_NULL_HANDLE);

    for (uint32 set = 0; set <= maxSet; ++set)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        auto setIt = bindingsBySet.find(set);

        if (setIt != bindingsBySet.end())
        {
            bindings.reserve(setIt->second.size());

            for (const auto& [bindingIndex, binding] : setIt->second)
            {
                bindings.push_back(binding);
            }
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                   .bindingCount = static_cast<uint32>(bindings.size()),
                                                   .pBindings    = bindings.empty() ? nullptr : bindings.data()};

        if (vkCreateDescriptorSetLayout(context.device, &layoutInfo, nullptr, &program.descriptorSetLayouts[set]) != VK_SUCCESS)
        {
            LOG_ERROR << "Failed to create descriptor set layout: " << set;

            for (auto layout : program.descriptorSetLayouts)
            {
                if (layout != VK_NULL_HANDLE)
                    vkDestroyDescriptorSetLayout(context.device, layout, nullptr);
            }

            program.descriptorSetLayouts.clear();
            return false;
        }

        LOG_DEBUG << "Created DescriptorSetLayout: set=" << set << ", bindings=" << bindings.size();
    }

    return true;
}

bool CreateGraphicsPipeline(VulkanContext& context, VulkanProgram& newProgram,
                            const VkPipelineShaderStageCreateInfo* shaderStages)
{
    const auto& pipelineConfig = newProgram.pipelineConfig;

    LOG_DEBUG << newProgram.pipelineConfig.shaders.at(ShaderType::VERTEX_SHADER)
              << ", topology = " << magic_enum::enum_name(pipelineConfig.inputAssembly.topology);

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = pipelineConfig.vertexLayout.stride;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    for (const auto& attr : pipelineConfig.vertexLayout.attributes)
    {
        attributeDescriptions.push_back({attr.location, 0, attr.format, attr.offset});
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (pipelineConfig.vertexLayout.attributes.empty())
    {
        vertexInputInfo.vertexBindingDescriptionCount   = 0;
        vertexInputInfo.pVertexBindingDescriptions      = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions    = nullptr;
    }
    else
    {
        vertexInputInfo.vertexBindingDescriptionCount   = 1;
        vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = pipelineConfig.vertexLayout.attributes.size();
        vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
    }

    const auto& inputAssemblyConfig = pipelineConfig.inputAssembly;
    const auto topology             = ToVkPrimitiveTopology(inputAssemblyConfig.topology);

    if (not topology)
    {
        LOG_ERROR << "Error: Unsupported topology type: " << magic_enum::enum_name(inputAssemblyConfig.topology) << "\n";
        return false;
    }

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology               = *topology,
        .primitiveRestartEnable = inputAssemblyConfig.primitiveRestartEnable ? VK_TRUE : VK_FALSE,
    };

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = nullptr;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = nullptr;

    const auto& config = pipelineConfig.rasterization;
    VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable        = config.depthClampEnable ? VK_TRUE : VK_FALSE,
        .rasterizerDiscardEnable = config.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE,
        .polygonMode             = ToVkPolygonMode(config.polygonMode),
        .cullMode                = ToVkCullMode(config.cullMode),
        .frontFace               = ToVkFrontFace(config.frontFace),
        .depthBiasEnable         = config.depthBiasEnable ? VK_TRUE : VK_FALSE,
        .depthBiasConstantFactor = config.depthBiasConstantFactor,
        .depthBiasClamp          = config.depthBiasClamp,
        .depthBiasSlopeFactor    = config.depthBiasSlopeFactor,
        .lineWidth               = config.lineWidth};

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    const auto& blendConfig = pipelineConfig.colorBlend;

    const auto srcColorFactor = ToVkBlendFactor(blendConfig.srcColorFactor);
    const auto dstColorFactor = ToVkBlendFactor(blendConfig.dstColorFactor);
    const auto colorOperation = ToVkBlendOperation(blendConfig.colorOperation);

    const auto srcAlphaFactor = ToVkBlendFactor(blendConfig.srcAlphaFactor);
    const auto dstAlphaFactor = ToVkBlendFactor(blendConfig.dstAlphaFactor);
    const auto alphaOperation = ToVkBlendOperation(blendConfig.alphaOperation);

    if (!srcColorFactor || !dstColorFactor || !colorOperation || !srcAlphaFactor || !dstAlphaFactor || !alphaOperation)
    {
        LOG_ERROR << "Invalid color blend configuration!";
        return false;
    }

    colorBlendAttachment.blendEnable         = blendConfig.blendEnable ? VK_TRUE : VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = *srcColorFactor;
    colorBlendAttachment.dstColorBlendFactor = *dstColorFactor;
    colorBlendAttachment.colorBlendOp        = *colorOperation;

    colorBlendAttachment.srcAlphaBlendFactor = *srcAlphaFactor;
    colorBlendAttachment.dstAlphaBlendFactor = *dstAlphaFactor;
    colorBlendAttachment.alphaBlendOp        = *alphaOperation;

    colorBlendAttachment.colorWriteMask =
        (blendConfig.writeRed ? VK_COLOR_COMPONENT_R_BIT : 0) | (blendConfig.writeGreen ? VK_COLOR_COMPONENT_G_BIT : 0) |
        (blendConfig.writeBlue ? VK_COLOR_COMPONENT_B_BIT : 0) | (blendConfig.writeAlpha ? VK_COLOR_COMPONENT_A_BIT : 0);

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable   = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments    = &colorBlendAttachment;

    if (not CreateDescriptorSetLayouts(context, newProgram))
        return false;

    std::vector<VkDescriptorPoolSize> poolSizes = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 12u * 1000u},
                                                   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 12u * 1000u},
                                                   {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 12u * 1000u}};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = 2000u;

    auto imageCount = context.swapChainImages.size();
    newProgram.descriptorPools.resize(imageCount);  // One pool per swapchain image
    for (auto i = 0u; i < imageCount; ++i)
    {
        LOG_DEBUG << "vkCreateDescriptorPool " << i;
        if (vkCreateDescriptorPool(context.device, &poolInfo, nullptr, &newProgram.descriptorPools[i]) != VK_SUCCESS)
        {
            LOG_ERROR << "Error: Failed to create VkDescriptorPool for frame " << i << "!\n";
            return false;
        }
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = static_cast<uint32_t>(newProgram.descriptorSetLayouts.size()),
        .pSetLayouts            = newProgram.descriptorSetLayouts.empty() ? nullptr : newProgram.descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr};

    if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &newProgram.layout) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to create Pipeline Layout!";
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = newProgram.layout;
    pipelineInfo.renderPass          = context.renderPass;
    pipelineInfo.subpass             = 0;

    const auto& depthConfig = pipelineConfig.depthStencil;
    VkPipelineDepthStencilStateCreateInfo depthStencil{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .depthTestEnable       = depthConfig.depthTestEnable ? VK_TRUE : VK_FALSE,
        .depthWriteEnable      = depthConfig.depthWriteEnable ? VK_TRUE : VK_FALSE,
        .depthCompareOp        = ToVkCompareOp(depthConfig.depthCompareOp),
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
    };

    if (depthConfig.depthTestEnable or depthConfig.depthWriteEnable)
    {
        pipelineInfo.pDepthStencilState = &depthStencil;
    }
    else
    {
        pipelineInfo.pDepthStencilState = nullptr;
    }

    LOG_DEBUG << "vkCreateGraphicsPipelines";
    if (vkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newProgram.pipeline) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to compile Graphics Pipeline!\n";
        return false;
    }

    return true;
}

void DestroyShaderModules(VulkanContext& context, VkShaderModule vertModule, VkShaderModule fragModule)
{
    if (fragModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(context.device, fragModule, nullptr);
    }

    if (vertModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(context.device, vertModule, nullptr);
    }
}  // namespace
}  // namespace

VulkanShaderManager::VulkanShaderManager(VulkanContext& vkContext)
    : vkContext_(vkContext)
{
}

void VulkanShaderManager::SetShadersFilesLocations(const std::filesystem::path& path)
{
    shadersFileLocation_ = path / "GLSL";
}

ID VulkanShaderManager::Create(ShaderProgramType type)
{
    auto maybePiplineConfig = GetSimpleForwardShaderFiles(type);

    if (not maybePiplineConfig)
        return {};

    const auto& shaderFiles = maybePiplineConfig->shaders;

    std::string vertName;
    std::string fragName;

    auto vertexShaderIter = shaderFiles.find(ShaderType::VERTEX_SHADER);

    if (vertexShaderIter != shaderFiles.end())
    {
        vertName = vertexShaderIter->second;
    }
    else
    {
        LOG_ERROR << "Shader not found";
        return {};
    }
    auto fragmentShaderIter = shaderFiles.find(ShaderType::FRAGMENT_SHADER);

    if (fragmentShaderIter != shaderFiles.end())
    {
        fragName = fragmentShaderIter->second;
    }
    else
    {
        LOG_ERROR << "Shader not found";
        return {};
    }

    LOG_DEBUG << "Compile shaderType: " << magic_enum::enum_name(type) << ", vertName: " << vertName
              << ", fragName: " << fragName;

    std::string vertCode;
    std::string fragCode;
    if (!LoadShaderSourceFiles(shadersFileLocation_, vertName, fragName, vertCode, fragCode))
    {
        return {};
    }

    std::vector<uint32_t> vertSpirv;
    std::vector<uint32_t> fragSpirv;
    if (!CompileShaderSources(vertName, fragName, vertCode, fragCode, vertSpirv, fragSpirv))
    {
        LOG_DEBUG << "Compile shaderType: " << magic_enum::enum_name(type) << "\n"
                  << "=================Vertex shader=================(" << vertName << ")\n"
                  << vertCode << "\n"
                  << "=================Fragment shader=================(" << fragName << ")\n"
                  << fragCode << "\n===================================================";
        return {};
    }

    VkShaderModule vertModule = VK_NULL_HANDLE;
    VkShaderModule fragModule = VK_NULL_HANDLE;
    if (!CreateShaderModules(vkContext_, vertSpirv, fragSpirv, vertModule, fragModule))
    {
        return {};
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertModule;
    vertShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragModule;
    fragShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VulkanProgram newProgram{};
    newProgram.pipelineConfig = *maybePiplineConfig;

    if (!CreatePipelineLayout(vkContext_, newProgram))
    {
        DestroyShaderModules(vkContext_, vertModule, fragModule);
        return {};
    }

    if (!CreateGraphicsPipeline(vkContext_, newProgram, shaderStages))
    {
        vkDestroyPipelineLayout(vkContext_.device, newProgram.layout, nullptr);
        DestroyShaderModules(vkContext_, vertModule, fragModule);
        return {};
    }

    DestroyShaderModules(vkContext_, vertModule, fragModule);

    const auto programId           = vkContext_.idPool.AccquireNewId(ObjectType::SHADER_PROGRAM);
    vkContext_.programs[programId] = std::move(newProgram);

    LOG_DEBUG << "Success: Shader and GPU pipeline state compiled successfully. Registered VulkanProgram with ID: " << programId;

    return programId;
}
void VulkanShaderManager::Delete(IdType id)
{
    auto iter = vkContext_.programs.find(id);

    if (iter == vkContext_.programs.end())
    {
        LOG_WARN << "Shader program with ID " << id << " does not exist!";
        return;
    }

    auto& program = iter->second;
    if (program.pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(vkContext_.device, program.pipeline, nullptr);
        program.pipeline = VK_NULL_HANDLE;
    }

    if (program.layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(vkContext_.device, program.layout, nullptr);
        program.layout = VK_NULL_HANDLE;
    }

    for (auto& pool : program.descriptorPools)
    {
        if (pool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(vkContext_.device, pool, nullptr);
            pool = VK_NULL_HANDLE;
        }
    }

    program.descriptorPools.clear();
    for (auto& layout : program.descriptorSetLayouts)
    {
        if (layout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(vkContext_.device, layout, nullptr);
            layout = VK_NULL_HANDLE;
        }
    }

    program.descriptorSetLayouts.clear();

    vkContext_.programs.erase(iter);
    vkContext_.idPool.ReleaseId(id);
    LOG_DEBUG << "Successfully deleted Vulkan shader program with ID: " << id;
}
}  // namespace GraphicsApi::Vulkan
