#include "VulkanApi.h"

#include <SDL2/SDL_vulkan.h>

#include <iostream>
#include <vector>

#include "VulkanApi/SdlVulkanApi.h"
#include <Logger/Log.h>

namespace GraphicsApi::VulkanApi
{
VulkanApi::VulkanApi()
    : windowApi_(std::make_unique<::VulkanApi::SdlVulkanApi>(vkContext))
{
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

void VulkanApi::CreateContext()
{
    windowApi_->CreateContext();
    PrintVersion();
    InitRendering();
}

void VulkanApi::InitRendering()
{
    // ==========================================
    // KROK 1: TWORZENIE RENDER PASS
    // ==========================================
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = vkContext.swapChainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR; // To wymusza czyszczenie kolorem!
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE; // Chcemy zachować wynik, by go wyświetlić
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Obraz gotowy do wysłania do SDL2

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
        LOG_ERROR << "Blad: Nie udalo sie utworzyc Render Pass.\n";
        return;
    }

    // ==========================================
    // KROK 2: TWORZENIE FRAMEBUFFERS
    // ==========================================
    vkContext.framebuffers.resize(vkContext.swapChainImageViews.size());

    for (size_t i = 0; i < vkContext.swapChainImageViews.size(); ++i)
    {
        VkImageView attachments[] = { vkContext.swapChainImageViews[i] };

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
            LOG_ERROR << "Blad: Nie udalo sie utworzyc Framebuffera dla indeksu " << i << "\n";
            return;
        }
    }

    // ==========================================
    // KROK 3: TWORZENIE COMMAND POOL & BUFFERS
    // ==========================================
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vkContext.graphicsFamilyIndex;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vkContext.device, &poolInfo, nullptr, &vkContext.commandPool) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie utworzyc Command Pool.\n";
        return;
    }

    vkContext.commandBuffers.resize(vkContext.framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = vkContext.commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32>(vkContext.commandBuffers.size());

    if (vkAllocateCommandBuffers(vkContext.device, &allocInfo, vkContext.commandBuffers.data()) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie zaalokowac Command Buffers.\n";
        return;
    }

    LOG_DEBUG << "Sukces: Render Pass, Framebuffers i Command Buffers gotowe!";
}

void VulkanApi::DeleteContext()
{
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

void VulkanApi::SetShadersFilesLocations(const std::filesystem::path&)
{
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
ID VulkanApi::CreateShader(ShaderProgramType)
{
    return 0;
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
void VulkanApi::UseShader(uint32)
{
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
void VulkanApi::RenderMesh(uint32)
{
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
}
void VulkanApi::RenderQuadTs()
{
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
}  // namespace GraphicsApi::VulkanApi
