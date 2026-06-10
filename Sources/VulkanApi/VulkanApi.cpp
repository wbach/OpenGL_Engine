#include "VulkanApi.h"

#include <Logger/Log.h>
#include <SDL2/SDL_vulkan.h>

#include <iostream>
#include <vector>

#include "Types.h"
#include "VulkanApi/SdlVulkanApi.h"
#include "VulkanApi/VulkanContext.hpp"
#include "VulkanShaderCompiler.h"

namespace GraphicsApi::Vulkan
{
namespace
{
bool CreateRenderPass(VulkanContext& vkContext)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = vkContext.swapChainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

    if (vkCreateRenderPass(vkContext.device, &renderPassInfo, nullptr, &vkContext.renderPass) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Render Pass.\n";
        return false;
    }

    return true;
}

bool CreateFramebuffers(VulkanContext& vkContext)
{
    vkContext.framebuffers.resize(vkContext.swapChainImageViews.size());

    for (size_t i = 0; i < vkContext.swapChainImageViews.size(); ++i)
    {
        VkImageView attachments[] = {vkContext.swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = vkContext.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = vkContext.swapChainExtent.width;
        framebufferInfo.height          = vkContext.swapChainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(vkContext.device, &framebufferInfo, nullptr, &vkContext.framebuffers[i]) != VK_SUCCESS)
        {
            LOG_ERROR << "Error: Failed to create Framebuffer for index " << i << "\n";
            return false;
        }
    }

    return true;
}

bool CreateCommandPoolAndBuffers(VulkanContext& vkContext)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vkContext.graphicsFamilyIndex;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vkContext.device, &poolInfo, nullptr, &vkContext.commandPool) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Command Pool.\n";
        return false;
    }

    vkContext.commandBuffers.resize(vkContext.framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = vkContext.commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32>(vkContext.commandBuffers.size());

    if (vkAllocateCommandBuffers(vkContext.device, &allocInfo, vkContext.commandBuffers.data()) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate Command Buffers.\n";
        return false;
    }

    return true;
}

bool InitializeRenderingResources(VulkanContext& vkContext)
{
    if (!CreateRenderPass(vkContext))
    {
        return false;
    }

    if (!CreateFramebuffers(vkContext))
    {
        return false;
    }

    if (!CreateCommandPoolAndBuffers(vkContext))
    {
        return false;
    }

    LOG_DEBUG << "Success: Render Pass, Framebuffers and Command Buffers are ready!";
    return true;
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
}  // namespace

VulkanApi::VulkanApi()
    : windowApi_(std::make_unique<SdlVulkanApi>(vkContext))
{
    LOG_DEBUG << "";
}

VulkanApi::~VulkanApi()
{
    DeleteContext();
}

IWindowApi& VulkanApi::GetWindowApi()
{
    return *windowApi_;
}

void VulkanApi::Init()
{
}

void VulkanApi::EndFrame()
{

    if (vkContext.swapChain == VK_NULL_HANDLE)
        return;

    uint32 imageIndex = 0;

    // 1. Pobranie indeksu wolnego obrazu ze Swapchaina
    VkResult result = vkAcquireNextImageKHR(vkContext.device, vkContext.swapChain, UINT64_MAX, vkContext.imageAvailableSemaphore,
                                            VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        windowApi_->RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR << "Blad podczas pobierania obrazu ze Swapchaina! Kod bledu: " << result;
        return;
    }

    RenderFrame(imageIndex);

    // 3. Przygotowanie prezentacji przetworzonego obrazu na ekranie
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Czekamy na semafor sygnalizujący zakończenie rysowania przez RenderFrame!
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &vkContext.renderFinishedSemaphore;

    VkSwapchainKHR swapChains[] = {vkContext.swapChain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;

    result = vkQueuePresentKHR(vkContext.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR)
    {
        windowApi_->RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Image presentation failed!\n";
    }

    // Czekamy na urządzenie przed kolejną klatką
    vkDeviceWaitIdle(vkContext.device);
}

void VulkanApi::CreateContext()
{
    windowApi_->CreateContext();
    PrintVersion();

    if (!InitializeRenderingResources(vkContext))
    {
        return;
    }
}

void VulkanApi::InitRendering()
{
    if (!InitializeRenderingResources(vkContext))
    {
        return;
    }
}

void VulkanApi::DeleteContext()
{
    for (std::pair<const IdType, VulkanProgram>& pair : vkContext.programs)
    {
        if (pair.second.pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(vkContext.device, pair.second.pipeline, nullptr);
        }
        if (pair.second.layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(vkContext.device, pair.second.layout, nullptr);
        }
    }
    vkContext.programs.clear();

    windowApi_->DeleteContext();
}

void VulkanApi::PrintVersion()
{
    if (vkContext.physicalDevice == VK_NULL_HANDLE)
    {
        std::cout << "Vulkan Device: Nie zainicjalizowano\n";
        return;
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(vkContext.physicalDevice, &deviceProperties);
    std::cout << "Vulkan Device: " << deviceProperties.deviceName << "\n";
}

void VulkanApi::SetShadersFilesLocations(const std::filesystem::path& path)
{
    shadersFileLocation_ = path / "VulkanApi" / "GLSL";
}
void VulkanApi::DebugNormalMeshGeneration(bool)
{
}
void VulkanApi::SetShaderQuaility(ShaderQuaility)
{
}
bool VulkanApi::IsTesselationSupported() const
{
    return false;
}
std::vector<RendererType> VulkanApi::GetSupportedRenderers() const
{
    return {RendererType::SIMPLE};
}
void VulkanApi::SetDefaultTarget()
{
}
void VulkanApi::SetBackgroundColor(const Color& color)
{
    vkContext.backgroundColor = color;
}
const Color& VulkanApi::GetBackgroundColor() const
{
    return vkContext.backgroundColor;
}
void VulkanApi::EnableDepthTest()
{
}
void VulkanApi::DisableDepthTest()
{
}
void VulkanApi::PrepareFrame()
{
}
ID VulkanApi::CreateShader(ShaderProgramType type)
{
    std::string vertPath = "";
    std::string fragPath = "";

    // if (type == ShaderProgramType::Loading)
    {
        vertPath = "triangle.vert";
        fragPath = "triangle.frag";
    }
    // else
    // {
    //     LOG_ERROR << "Nieobsługiwany typ programu shaderów!\n";
    //     return {};
    // }

    auto vertPathFullPath = std::filesystem::absolute(shadersFileLocation_ / vertPath).lexically_normal();
    auto fragPathFullPath = std::filesystem::absolute(shadersFileLocation_ / fragPath).lexically_normal();

    std::string vertCode = ReadTextFile(vertPathFullPath.string());
    std::string fragCode = ReadTextFile(fragPathFullPath.string());

    if (vertCode.empty() or fragCode.empty())
    {
        LOG_ERROR << "Error: Shader source is empty or the files could not be opened.\n";
        return {};
    }

    std::vector<uint32_t> vertSpirv = CompileGlslToSpirv(vertPath, shaderc_vertex_shader, vertCode);
    std::vector<uint32_t> fragSpirv = CompileGlslToSpirv(fragPath, shaderc_fragment_shader, fragCode);

    if (vertSpirv.empty() or fragSpirv.empty())
    {
        LOG_ERROR << "Stopping shader loading due to compilation errors.\n";
        return {};
    }

    VkShaderModule vertModule = CreateShaderModule(vkContext, vertSpirv);
    VkShaderModule fragModule = CreateShaderModule(vkContext, fragSpirv);

    if (vertModule == VK_NULL_HANDLE or fragModule == VK_NULL_HANDLE)
    {
        LOG_ERROR << "Error: Failed to allocate VkShaderModule.\n";
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
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(vkContext.swapChainExtent.width);
    viewport.height   = static_cast<float>(vkContext.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vkContext.swapChainExtent;

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
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
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

    VulkanProgram newProgram{};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(vkContext.device, &pipelineLayoutInfo, nullptr, &newProgram.layout) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Pipeline Layout!\n";
        vkDestroyShaderModule(vkContext.device, fragModule, nullptr);
        vkDestroyShaderModule(vkContext.device, vertModule, nullptr);
        return {};
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
    pipelineInfo.renderPass          = vkContext.renderPass;  // Powiązanie z naszym Render Passem czyszczącym okno
    pipelineInfo.subpass             = 0;

    if (vkCreateGraphicsPipelines(vkContext.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newProgram.pipeline) !=
        VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to compile Graphics Pipeline!\n";
        vkDestroyPipelineLayout(vkContext.device, newProgram.layout, nullptr);
        vkDestroyShaderModule(vkContext.device, fragModule, nullptr);
        vkDestroyShaderModule(vkContext.device, vertModule, nullptr);
        return {};
    }

    vkDestroyShaderModule(vkContext.device, fragModule, nullptr);
    vkDestroyShaderModule(vkContext.device, vertModule, nullptr);

    auto programId                = vkContext.programsPoolId.getId();
    vkContext.programs[programId] = std::move(newProgram);

    LOG_DEBUG << "Sukces: Shader i stan maszyny GPU skompilowane! Zarejestrowano VulkanProgram pod ID: " << programId;

    return programId;
}
ID VulkanApi::CreateShaderBuffer(uint32, uint32, DrawFlag)
{
    return 0;
}
ID VulkanApi::CreateShaderStorageBuffer(uint32, uint32, DrawFlag)
{
    return 0;
}
void VulkanApi::UpdateShaderBuffer(uint32, void const*)
{
}
void VulkanApi::UpdateShaderStorageBuffer(uint32, void const*, uint32)
{
}
void* VulkanApi::MapShaderStorageBuffer(uint32, uint32, uint32)
{
    return nullptr;
}
void VulkanApi::UnmapShaderStorageBuffer(uint32)
{
}
uint32 VulkanApi::BindShaderBuffer(uint32)
{
    return 0;
}
void VulkanApi::UseShader(uint32 shaderId)
{
    if (vkContext.programs.find(shaderId) == vkContext.programs.end())
    {
        LOG_ERROR << "Attempt to activate a non-existent shader/pipeline with ID: " << shaderId << "\n";
        activePipelineId.reset();
        return;
    }

    activePipelineId = shaderId;
}
ID VulkanApi::CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap)
{
    return 0;
}
ID VulkanApi::CreateTexture(const std::vector<Utils::Image>&, TextureFilter, TextureMipmap)
{
    return 0;
}
ID VulkanApi::CreateTextureStorage(TextureType, TextureFilter, int32)
{
    return 0;
}
ID VulkanApi::CreateCubMapTexture(const CubemapImages&)
{
    return 0;
}
void VulkanApi::GenerateMipmaps(IdType)
{
}
void VulkanApi::CopyTexture(IdType, IdType, uint32, uint32)
{
}
void VulkanApi::UpdateTexture(uint32, const vec2ui&, const Utils::Image&)
{
}
void VulkanApi::UpdateTexture(uint32, const Utils::Image&)
{
}
void VulkanApi::ClearTexture(uint32, const Color&)
{
}
void VulkanApi::EnableBlend()
{
}
void VulkanApi::DisableBlend()
{
}
void VulkanApi::EnableDepthMask()
{
}
void VulkanApi::DisableDepthMask()
{
}
void VulkanApi::ActiveTexture(uint32, uint32)
{
}
void VulkanApi::ActiveTexture(uint32)
{
}
void VulkanApi::DeleteObject(uint32)
{
}
void VulkanApi::DeleteObject(const std::vector<uint32>&)
{
}
void VulkanApi::DeleteShaderBuffer(uint32)
{
}
void VulkanApi::DeleteShaderBuffer(const std::vector<uint32>&)
{
}
std::string VulkanApi::GetBufferStatus()
{
    return "";
}
ID VulkanApi::CreatePatchMesh(const std::vector<float>&)
{
    return 0;
}
ID VulkanApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return 0;
}
ID VulkanApi::CreateMesh(const MeshRawData&, RenderType)
{
    return 0;
}
ID VulkanApi::CreateDynamicLineMesh()
{
    return 0;
}
ID VulkanApi::CreateParticle()
{
    return 0;
}
ID VulkanApi::CreateAnimatedParticle()
{
    return 0;
}
void VulkanApi::Compute(uint32, uint32, uint32)
{
}
void VulkanApi::RenderPurePatchedMeshInstances(uint32)
{
}
void VulkanApi::RenderMesh(uint32 meshId)
{
    if (not activePipelineId.has_value())
    {
        LOG_WARN << "Attempt to call RenderMesh without an active shader set (UseShader)!\n";
        return;
    }

    auto it = vkContext.programs.find(*activePipelineId);

    if (it != vkContext.programs.end())
    {
        VulkanDrawCall drawCall{};
        drawCall.meshId = meshId;

        it->second.drawCalls.push_back(drawCall);
    }
    else
    {
        LOG_ERROR << "Error: The active shader ID (" << activePipelineId << ") does not exist in the Vulkan context!\n";
    }
}
void VulkanApi::RenderProcedural(uint32)
{
}
void VulkanApi::RenderDebugNormals(uint32)
{
}
void VulkanApi::RenderTriangleStripMesh(uint32)
{
}
void VulkanApi::RenderMeshInstanced(uint32, uint32)
{
}
void VulkanApi::RenderPoints(uint32)
{
}
void VulkanApi::RenderQuad()
{
    if (not activePipelineId.has_value())
    {
        return;
    }

    std::unordered_map<IdType, VulkanProgram>::iterator it = vkContext.programs.find(*activePipelineId);

    if (it != vkContext.programs.end())
    {
        VulkanDrawCall drawCall{};
        drawCall.meshId = 0;

        it->second.drawCalls.push_back(drawCall);
    }
    else
    {
        LOG_ERROR << "Error: RenderQuad is trying to use a non-existent active shader ID: " << activePipelineId << "\n";
    }
}
void VulkanApi::RenderQuadTs()
{
    RenderQuad();
}
void VulkanApi::EnableClipingPlane(uint32)
{
}
void VulkanApi::DisableCliping(uint32)
{
}
void VulkanApi::EnableCulling()
{
}
void VulkanApi::DisableCulling()
{
}
void VulkanApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void VulkanApi::BindTexture(uint32) const
{
}
void VulkanApi::BindImageTexture(uint32, TextureAccess)
{
}
ID VulkanApi::CreateShadowMap(uint32, uint32)
{
    return 0;
}
void VulkanApi::PolygonModeRender()
{
}
void VulkanApi::LineModeRender()
{
}
void VulkanApi::SetBlendFunction(BlendFunctionType)
{
}
void VulkanApi::UpdateMatrixes(uint32, const std::vector<mat4>&)
{
}
void VulkanApi::UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&)
{
}
void VulkanApi::UpdateLineMesh(uint32, const LineMesh&)
{
}
void VulkanApi::UpdateOffset(uint32, const std::vector<vec4>&)
{
}
void VulkanApi::UpdateBlend(uint32, const std::vector<float>&)
{
}
void VulkanApi::LoadViewMatrix(const mat4&)
{
}
void VulkanApi::LoadProjectionMatrix(const mat4&)
{
}
void VulkanApi::DrawLine(const vec3&, const vec3&, const vec3&)
{
}
mat4 VulkanApi::PrepareMatrixToLoad(const mat4& matrix)
{
    return matrix;
}
std::vector<uint8> VulkanApi::GetTextureData(uint32) const
{
    return {};
}
const TextureInfo& VulkanApi::GetTextureInfo(uint32) const
{
    return dummyTextureInfo;
}
void VulkanApi::TakeSnapshoot(const std::filesystem::path&) const
{
}
std::optional<Utils::Image> VulkanApi::GetImage(IdType) const
{
    return std::nullopt;
}
std::vector<Utils::Image> VulkanApi::GetImageArray(IdType) const
{
    return {};
}
IFrameBuffer& VulkanApi::GetDefaultFrameBuffer()
{
    return *dummyFrameBuffer;
}
IFrameBuffer& VulkanApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&)
{
    return *dummyFrameBuffer;
}
void VulkanApi::DeleteFrameBuffer(IFrameBuffer&)
{
}
void VulkanApi::RenderFrame(uint32 imageIndex)
{
    VkCommandBuffer commandBuffer = vkContext.commandBuffers[imageIndex];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to begin recording the Command Buffer!\n";
        return;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = vkContext.renderPass;
    renderPassInfo.framebuffer       = vkContext.framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkContext.swapChainExtent;

    // Pobieramy kolor tła z konfiguracji silnika
    VkClearValue clearColor        = {{{vkContext.backgroundColor[0], vkContext.backgroundColor[1], vkContext.backgroundColor[2],
                                        vkContext.backgroundColor[3]}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    // Otwieramy Render Pass (Karta graficzna czyści ekran kolorem tła)
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // RYSOWANIE GEOMETRII
    for (std::pair<const IdType, VulkanProgram>& pair : vkContext.programs)
    {
        VulkanProgram& program = pair.second;

        if (program.drawCalls.empty())
        {
            continue;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipeline);

        for (const VulkanDrawCall& drawCall : program.drawCalls)
        {
            (void)drawCall;
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
            // vkCmdDraw(commandBuffer, program.vertexCount, 1, 0, 0);
        }

        program.drawCalls.clear();
    }

    // Dopiero po narysowaniu wszystkich obiektów zamykamy Render Pass!
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to finish recording the Command Buffer!\n";
        return;
    }

    // WYSYŁKA DO GPU Z PEŁNĄ SYNCHRONIZACJĄ SEMAFORÓW
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Czekaj z rysowaniem, aż obraz będzie wolny (imageAvailable)
    VkSemaphore waitSemaphores[]      = {vkContext.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    // Sygnalizuj semafor renderFinished, kiedy GPU skończy rysować geometrię
    VkSemaphore signalSemaphores[]  = {vkContext.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    if (vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        LOG_ERROR << "Error while submitting render commands to the queue (vkQueueSubmit)!\n";
        return;
    }
}
}  // namespace GraphicsApi::Vulkan
