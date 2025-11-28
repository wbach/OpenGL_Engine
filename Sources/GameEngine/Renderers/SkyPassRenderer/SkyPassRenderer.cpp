#include "SkyPassRenderer.h"

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

namespace GameEngine
{
namespace
{
struct SkyPassBuffer
{
    mat4 invProj;
    mat4 invViewRot;
    AlignWrapper<vec4> screenSize;
    AlignWrapper<vec4> sunDirection;
    AlignWrapper<vec4> sunColor;  // w is night
};

}  // namespace
SkyPassRenderer::SkyPassRenderer(RendererContext& context)
    : context(context)
    , shader(context.graphicsApi_, GraphicsApi::ShaderProgramType::Sky)
{
}
void SkyPassRenderer::Init()
{
    if (not context.camera_)
    {
        LOG_ERROR << "Camera not set during intialization!";
        return;
    }
    shader.Init();

    if (not bufferId)
    {
        const uint32 BIND_LOCATION{6};
        bufferId = context.graphicsApi_.CreateShaderBuffer(BIND_LOCATION, sizeof(SkyPassBuffer));
    }

    if (not frameBuffer)
    {
        using namespace GraphicsApi::FrameBuffer;
        const uint32 renderingScale = 1;
        const auto& camera          = *context.camera_;
        frameBufferSize             = camera.GetProjection().GetRenderingSize() / renderingScale;
        Attachment color(*frameBufferSize, Type::Color0, Format::Rgba32f);

        frameBuffer      = &context.graphicsApi_.CreateFrameBuffer({color});
        isReady          = frameBuffer->Init();
        auto atachmentId = frameBuffer->GetAttachmentTexture(Type::Color0);
        context.sharedTextures[magic_enum::enum_index(SharedTextures::skyTexture).value()] = atachmentId;
        LOG_DEBUG << "Framebuffer for sky created. Id: " << frameBuffer->GetId() << ", ColorAttachmentId = " << atachmentId;
    }
}
void SkyPassRenderer::Render(uint32 depthTextureId)
{
    if (not isReady)
        return;

    if (not context.camera_)
    {
        LOG_ERROR << "Camera not set during render!";
        return;
    }

    SkyPassBuffer buffer;
    buffer.invProj    = glm::inverse(context.camera_->GetProjectionMatrix());
    buffer.invViewRot = glm::inverse(context.camera_->GetViewMatrix());
    buffer.screenSize = vec4(frameBufferSize->x, frameBufferSize->y, 0.0, 0.0);

    auto directionalLights =
        context.scene_->getComponentController().GetAllActiveComponentsOfType<Components::DirectionalLightComponent>();

    if (not directionalLights.empty())
    {
        const auto& directionalLight = directionalLights.front();
        buffer.sunDirection          = vec4(glm::normalize(directionalLight->GetDirection()), 1.0f);
        buffer.sunColor = vec4(directionalLight->color.xyz(), context.scene_->GetDayNightCycle().GetDayNightBlendFactor());
    }
    else
    {
        buffer.sunDirection = vec4(0.f);
        buffer.sunColor = vec4(0.f);
    }

    context.graphicsApi_.SetViewPort(0, 0, frameBufferSize->x, frameBufferSize->y);

    frameBuffer->Clear();
    frameBuffer->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    shader.Start();

    context.graphicsApi_.UpdateShaderBuffer(*bufferId, &buffer);
    context.graphicsApi_.BindShaderBuffer(*bufferId);

    context.graphicsApi_.ActiveTexture(0, depthTextureId);
    context.graphicsApi_.RenderQuad();
    shader.Stop();
    frameBuffer->UnBind();
}
void SkyPassRenderer::ReloadShaders()
{
    shader.Reload();
}
SkyPassRenderer::~SkyPassRenderer()
{
    CleanUp();
}
void SkyPassRenderer::CleanUp()
{
    if (bufferId)
    {
        context.graphicsApi_.DeleteShaderBuffer(*bufferId);
        bufferId.reset();
    }

    shader.Clear();

    if (frameBuffer)
    {
        context.graphicsApi_.DeleteFrameBuffer(*frameBuffer);
        frameBuffer = nullptr;
    }
}
}  // namespace GameEngine
