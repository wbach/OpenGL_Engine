#include "SkyPassRenderer.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Types.h"
#include "glm/matrix.hpp"
#include "magic_enum/magic_enum.hpp"

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
    AlignWrapper<vec4> sunColor; //w is night
};

}  // namespace
SkyPassRenderer::SkyPassRenderer(RendererContext& context)
    : context(context)
    , shader(context.graphicsApi_, GraphicsApi::ShaderProgramType::Sky)
{
}
void SkyPassRenderer::Init()
{
    shader.Init();

    if (not bufferId)
    {
        const uint32 BIND_LOCATION{6};
        bufferId = context.graphicsApi_.CreateShaderBuffer(BIND_LOCATION, sizeof(SkyPassBuffer));
    }

    using namespace GraphicsApi::FrameBuffer;
    const uint32 renderingScale = 1;
    frameBufferSize             = context.projection_.GetRenderingSize() / renderingScale;
    Attachment color(*frameBufferSize, Type::Color0, Format::Rgba32f);

    frameBuffer      = &context.graphicsApi_.CreateFrameBuffer({color});
    isReady          = frameBuffer->Init();
    auto atachmentId = frameBuffer->GetAttachmentTexture(Type::Color0);
    context.sharedTextures[magic_enum::enum_index(SharedTextures::skyTexture).value()] = atachmentId;
    LOG_DEBUG << "Framebuffer for sky created. Id: " << frameBuffer->GetId() << ", ColorAttachmentId = " << atachmentId;
}
void SkyPassRenderer::Prepare()
{
}
void SkyPassRenderer::Render(uint32 depthTextureId)
{
    if (not isReady)
        return;

    context.graphicsApi_.SetViewPort(0, 0, frameBufferSize->x, frameBufferSize->y);

    frameBuffer->Clear();
    frameBuffer->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    shader.Start();

    SkyPassBuffer buffer;
    buffer.invProj       = glm::inverse(context.projection_.GetProjectionMatrix());
    buffer.invViewRot    = glm::inverse(context.scene_->GetCamera().GetViewMatrix());
    buffer.screenSize    = vec4(frameBufferSize->x, frameBufferSize->y, 0.0, 0.0);
    const auto& dirLight = context.scene_->GetDirectionalLight();
    buffer.sunDirection  = vec4(glm::normalize(dirLight.GetDirection()), 0.0f);
    buffer.sunColor      = vec4(dirLight.GetColour(), context.scene_->GetDayNightCycle().GetDayNightBlendFactor());

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
}  // namespace GameEngine
