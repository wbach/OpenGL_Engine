#include "VulkanShaderManager.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <vector>

#include "GraphicsApi/ShaderProgramType.h"
#include "Types.h"
#include "VulkanShaderCompiler.h"
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

std::pair<std::string, std::string> GetShaderSourceNames(ShaderProgramType type)
{
    if (type == ShaderProgramType::Entity)
    {
        return {"SimpleEntityShader.vert", "SimpleEntityShader.frag"};
    }
    return {"triangle.vert", "triangle.frag"};
}

bool LoadShaderSourceFiles(const std::filesystem::path& shaderDirectory, const std::string& vertName, const std::string& fragName,
                           std::string& vertCode, std::string& fragCode)
{
    const auto vertPathFullPath = std::filesystem::absolute(shaderDirectory / vertName).lexically_normal();
    const auto fragPathFullPath = std::filesystem::absolute(shaderDirectory / fragName).lexically_normal();

    vertCode = Utils::ReadFilesWithIncludes(vertPathFullPath);
    fragCode = Utils::ReadFilesWithIncludes(fragPathFullPath);

    LOG_DEBUG << "Vertex vertCode: " << vertCode;
    LOG_DEBUG << "Fragment fragCode: " << fragCode;

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
                            const VkPipelineShaderStageCreateInfo* shaderStages)
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = 19 * sizeof(float);  // size wszystkich (3 + 2 + 3 + 3 + 4 + 4)
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescriptions[6]{};
    // 0: Position (vec3)
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = 0;
    // 1: TexCoord (vec2)
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset   = 3 * sizeof(float);
    // 2: Normal (vec3)
    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset   = (3 + 2) * sizeof(float);
    // 3: Tangent (vec3)
    attributeDescriptions[3].binding  = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset   = (3 + 2 + 3) * sizeof(float);
    // 4: Weights (vec4)
    attributeDescriptions[4].binding  = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[4].offset   = (3 + 2 + 3 + 3) * sizeof(float);
    // 5: BoneIds (ivec4)
    attributeDescriptions[5].binding  = 0;
    attributeDescriptions[5].location = 5;
    attributeDescriptions[5].format   = VK_FORMAT_R32G32B32A32_SINT;
    attributeDescriptions[5].offset   = (3 + 2 + 3 + 3 + 4) * sizeof(float);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 6;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // After openglowskie Y+ w górę, musimy odwrócić viewport, aby renderować poprawnie
    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = static_cast<float>(context.swapChainExtent.height);
    viewport.width    = static_cast<float>(context.swapChainExtent.width);
    viewport.height   = -static_cast<float>(context.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = context.swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

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

    const auto globalStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    auto bindings           = std::vector<VkDescriptorSetLayoutBinding>(9);

    for (uint32_t i = 0; i < 9; ++i)
    {
        bindings[i].binding            = i;
        bindings[i].descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[i].descriptorCount    = 1;
        bindings[i].stageFlags         = globalStages;
        bindings[i].pImmutableSamplers = nullptr;
    }

    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
    descriptorLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorLayoutInfo.pBindings    = bindings.data();

    if (vkCreateDescriptorSetLayout(context.device, &descriptorLayoutInfo, nullptr, &newProgram.descriptorSetLayout) !=
        VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Descriptor Set Layout for Program!\n";
        return false;
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 9u * 1000u;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1u;
    poolInfo.pPoolSizes    = &poolSize;
    poolInfo.maxSets       = 1000u;

    if (vkCreateDescriptorPool(context.device, &poolInfo, nullptr, &newProgram.descriptorPool) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create VkDescriptorPool!\n";
        return {};
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = newProgram.descriptorPool;
    allocInfo.descriptorSetCount = 1u;
    allocInfo.pSetLayouts        = &newProgram.descriptorSetLayout;

    if (vkAllocateDescriptorSets(context.device, &allocInfo, &newProgram.descriptorSet) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate VkDescriptorSet!\n";
        vkDestroyDescriptorPool(context.device, newProgram.descriptorPool, nullptr);
        return {};
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 1;
    pipelineLayoutInfo.pSetLayouts            = &newProgram.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges    = nullptr;

    if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &newProgram.layout) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Pipeline Layout!\n";
        vkDestroyDescriptorPool(context.device, newProgram.descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(context.device, newProgram.descriptorSetLayout, nullptr);
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
    pipelineInfo.layout              = newProgram.layout;
    pipelineInfo.renderPass          = context.renderPass;
    pipelineInfo.subpass             = 0;

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
    shadersFileLocation_ = path / "VulkanApi" / "GLSL";
}

ID VulkanShaderManager::Create(ShaderProgramType type)
{
    const auto [vertName, fragName] = GetShaderSourceNames(type);

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

    if (!CreateGraphicsPipeline(vkContext_, newProgram, shaderStages))
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
