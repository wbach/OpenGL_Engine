#include "LightShaftOclusionRenderer.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Logger/Log.h>
#include <Types.h>

#include <glm/matrix.hpp>
#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Lights/DirectionalLightComponent.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GraphicsApi/BufferParamters.h"
#include "GraphicsApi/TextureParamters.h"

namespace GameEngine
{
namespace
{
const int NUM_SAMPLES = 100;
struct SunBuffer
{
    AlignWrapper<vec4> ndcSunPosition;                             // z - turn off, w -samples
    AlignWrapper<vec4> sunColor{vec4{1.f}};                        // w is night
    AlignWrapper<vec4> blurParams{vec4{0.2f, 0.97f, 2.0f, 0.05f}};  // x- exposure, y - decay, z - density, w - weight
};

}  // namespace
LightShaftOclusionRenderer::LightShaftOclusionRenderer(RendererContext& context)
    : context(context)
    , oclusionShader(context.graphicsApi_, GraphicsApi::ShaderProgramType::LightShaftOclusion)
    , blurShader(context.graphicsApi_, GraphicsApi::ShaderProgramType::LightShaftBlure)
{
}
void LightShaftOclusionRenderer::Init()
{
    if (not context.camera_)
    {
        LOG_ERROR << "Camera not set during intialization!";
        return;
    }
    oclusionShader.Init();
    blurShader.Init();

    if (not sunBufferId)
    {
        const uint32 BIND_LOCATION{6};
        sunBufferId = context.graphicsApi_.CreateShaderBuffer(BIND_LOCATION, sizeof(SunBuffer), GraphicsApi::DrawFlag::Dynamic);
    }

    const uint32 renderingScale = 1;
    const auto& camera          = *context.camera_;
    frameBufferSize             = camera.GetProjection().GetRenderingSize() / renderingScale;

    if (not oclusionFrameBuffer)
    {
        oclusionFrameBuffer = createFrameBuffer();

        if (oclusionFrameBuffer)
            oclusionTextureId = oclusionFrameBuffer->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
    }

    if (not blurFrameBuffer)
    {
        blurFrameBuffer = createFrameBuffer();

        if (blurFrameBuffer)
        {
            auto atachmentId = blurFrameBuffer->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
            context.sharedTextures[magic_enum::enum_index(SharedTextures::lightShafts).value()] = atachmentId;
        }
    }
}
void LightShaftOclusionRenderer::Render(uint32 depthTextureId)
{
    if (not oclusionFrameBuffer or not blurFrameBuffer)
        return;

    if (not context.camera_)
    {
        LOG_ERROR << "Camera not set during render!";
        return;
    }

    auto directionalLights =
        context.scene_->getComponentController().GetAllActiveComponentsOfType<Components::DirectionalLightComponent>();

    SunBuffer buffer;
    if (not directionalLights.empty())
    {
        const auto& directionalLight = directionalLights.front();
        buffer.ndcSunPosition        = calculateSunPosition(*directionalLight);
        buffer.sunColor = vec4(directionalLight->color.xyz(), context.scene_->GetDayNightCycle().GetDayNightBlendFactor());
    }
    else
    {
        buffer.ndcSunPosition = vec4(0.f);
        buffer.sunColor       = vec4(0.f);
    }

    context.graphicsApi_.UpdateShaderBuffer(*sunBufferId, &buffer);
    context.graphicsApi_.BindShaderBuffer(*sunBufferId);
    context.graphicsApi_.SetViewPort(0, 0, frameBufferSize->x, frameBufferSize->y);

    Render(oclusionShader, *oclusionFrameBuffer, depthTextureId);
    Render(blurShader, *blurFrameBuffer, *oclusionTextureId);
}
void LightShaftOclusionRenderer::Render(ShaderProgram& shader, GraphicsApi::IFrameBuffer& buffer, uint32 textureId)
{
    buffer.Clear();
    buffer.Bind(GraphicsApi::FrameBuffer::BindType::Write);
    shader.Start();

    context.graphicsApi_.ActiveTexture(0, textureId);
    context.graphicsApi_.RenderProcedural(3);
    shader.Stop();
    buffer.UnBind();
}
void LightShaftOclusionRenderer::ReloadShaders()
{
    oclusionShader.Reload();
    blurShader.Reload();
}
LightShaftOclusionRenderer::~LightShaftOclusionRenderer()
{
    CleanUp();
}
void LightShaftOclusionRenderer::CleanUp()
{
    if (sunBufferId)
    {
        context.graphicsApi_.DeleteShaderBuffer(*sunBufferId);
        sunBufferId.reset();
    }

    oclusionShader.Clear();
    blurShader.Clear();

    if (oclusionFrameBuffer)
    {
        context.graphicsApi_.DeleteFrameBuffer(*oclusionFrameBuffer);
        oclusionFrameBuffer = nullptr;
    }

    if (blurFrameBuffer)
    {
        context.graphicsApi_.DeleteFrameBuffer(*blurFrameBuffer);
        blurFrameBuffer = nullptr;
    }
}
vec4 LightShaftOclusionRenderer::calculateSunPosition(const Components::DirectionalLightComponent& light)
{
    auto sunPosWorld  = context.camera_->GetPosition() - (light.GetDirection() * 1000.0f);
    auto clipSpacePos = context.camera_->GetProjectionViewMatrix() * glm::vec4(sunPosWorld, 1.0f);

    if (clipSpacePos.w > 0.0f)
    {
        auto ndc            = vec3(clipSpacePos) / clipSpacePos.w;
        auto lightScreenPos = glm::vec2(ndc.x, ndc.y) * 0.5f + 0.5f;
        return vec4(lightScreenPos, 1.f, NUM_SAMPLES);
    }
    else
    {
        return vec4(0.f, 0.f, 0.f, NUM_SAMPLES);
    }
}
GraphicsApi::IFrameBuffer* LightShaftOclusionRenderer::createFrameBuffer()
{
    using namespace GraphicsApi::FrameBuffer;

    Attachment color(*frameBufferSize, Type::Color0, Format::Rgba32f);
    color.filter =  GraphicsApi::FrameBuffer::Filter::Linear;

    auto frameBuffer = &context.graphicsApi_.CreateFrameBuffer({color});
    if (not frameBuffer->Init())
    {
        LOG_DEBUG << "Framebuffer creation error";
        context.graphicsApi_.DeleteFrameBuffer(*frameBuffer);
        return nullptr;
    }

    return frameBuffer;
}
}  // namespace GameEngine
