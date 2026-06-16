#include "VulkanShaderManager.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
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

bool CreateGraphicsPipeline(VulkanContext& context, VulkanProgram& newProgram,
                            const VkPipelineShaderStageCreateInfo* shaderStages, const PipelineConfig& pipelineConfig)
{
    LOG_DEBUG << pipelineConfig.shaders.at(ShaderType::VERTEX_SHADER)
              << ", topology = " << magic_enum::enum_name(pipelineConfig.topology);

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

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext                  = nullptr;
    inputAssembly.flags                  = 0;
    inputAssembly.primitiveRestartEnable = pipelineConfig.primitiveRestartEnable ? VK_TRUE : VK_FALSE;

    switch (pipelineConfig.topology)
    {
        case Topology::TriangleList:
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case Topology::TriangleStrip:
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
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
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    LOG_DEBUG << "inputAssembly.topology " << inputAssembly.topology;
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

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable   = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments    = &colorBlendAttachment;

    const auto globalStages           = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    const uint32_t totalBindingsCount = 12;
    auto bindings                     = std::vector<VkDescriptorSetLayoutBinding>(totalBindingsCount);

    for (uint32_t i = 0; i < totalBindingsCount; ++i)
    {
        bindings[i].binding            = i;
        bindings[i].descriptorCount    = 1;
        bindings[i].stageFlags         = globalStages;
        bindings[i].pImmutableSamplers = nullptr;

        auto it = pipelineConfig.bufferLayout.find(i);
        if (it != pipelineConfig.bufferLayout.end() && it->second == GraphicsApi::DrawFlag::Dynamic)
        {
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        }
        else
        {
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
    descriptorLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorLayoutInfo.pBindings    = bindings.data();
    LOG_DEBUG << "vkCreateDescriptorSetLayout";
    if (vkCreateDescriptorSetLayout(context.device, &descriptorLayoutInfo, nullptr, &newProgram.descriptorSetLayout) !=
        VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Descriptor Set Layout for Program!\n";
        return false;
    }

    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 12u * 1000u},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 12u * 1000u}
    };

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

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 1;
    pipelineLayoutInfo.pSetLayouts            = &newProgram.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges    = nullptr;
    LOG_DEBUG << "vkCreatePipelineLayout";
    if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &newProgram.layout)  != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Pipeline Layout!\n";
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

    VkPipelineDepthStencilStateCreateInfo depthStencil{.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                                                       .depthTestEnable       = VK_TRUE,
                                                       .depthWriteEnable      = VK_TRUE,
                                                       .depthCompareOp        = VK_COMPARE_OP_LESS,
                                                       .depthBoundsTestEnable = VK_FALSE,
                                                       .stencilTestEnable     = VK_FALSE};
    // if (pipelineConfig.depthTestEnable)
    {
        pipelineInfo.pDepthStencilState = &depthStencil;
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
}
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

    if (!CreatePipelineLayout(vkContext_, newProgram))
    {
        DestroyShaderModules(vkContext_, vertModule, fragModule);
        return {};
    }

    if (!CreateGraphicsPipeline(vkContext_, newProgram, shaderStages, *maybePiplineConfig))
    {
        vkDestroyPipelineLayout(vkContext_.device, newProgram.layout, nullptr);
        DestroyShaderModules(vkContext_, vertModule, fragModule);
        return {};
    }

    DestroyShaderModules(vkContext_, vertModule, fragModule);

    const auto programId           = vkContext_.programsPoolId.getId();
    vkContext_.programs[programId] = std::move(newProgram);

    LOG_DEBUG << "Success: Shader and GPU pipeline state compiled successfully. Registered VulkanProgram with ID: " << programId;

    return programId;
}
}  // namespace GraphicsApi::Vulkan
