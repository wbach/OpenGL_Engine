#include "ForwadSkyRenderer.h"

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Lights/DirectionalLightComponent.h"
#include "GameEngine/Renderers/SkyPassRenderer/SkyPassBuffer.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
ForwadSkyRenderer::ForwadSkyRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SimpleSky)
{
}
void ForwadSkyRenderer::init()
{
    shader_.Init();

    if (not bufferId)
    {
        const uint32 BIND_LOCATION{6};
        bufferId = context_.graphicsApi_.CreateShaderBuffer(BIND_LOCATION, sizeof(SkyPassBuffer), GraphicsApi::DrawFlag::Dynamic);
    }
}
void ForwadSkyRenderer::reloadShaders()
{
    shader_.Reload();
}
void ForwadSkyRenderer::render()
{
    if (not bufferId)
        return;

    updateBuffer();
    context_.graphicsApi_.BindShaderBuffer(*bufferId);

    shader_.Start();
    context_.graphicsApi_.DisableDepthTest();
    context_.graphicsApi_.RenderProcedural(3);
    context_.graphicsApi_.EnableDepthTest();
}
void ForwadSkyRenderer::updateBuffer()
{
    SkyPassBuffer buffer;
    buffer.invProj         = glm::inverse(context_.camera_->GetProjectionMatrix());
    buffer.invViewRot      = glm::inverse(context_.camera_->GetViewMatrix());
    const auto& windowSize = context_.graphicsApi_.GetWindowApi().GetWindowSize();
    buffer.screenSize      = vec4(windowSize.x, windowSize.y, 0.0, 0.0);

    auto directionalLights =
        context_.scene_->getComponentController().GetAllActiveComponentsOfType<Components::DirectionalLightComponent>();

    if (not directionalLights.empty())
    {
        const auto& directionalLight = directionalLights.front();
        buffer.sunDirection          = vec4(glm::normalize(directionalLight->GetDirection()), 1.0f);
        buffer.sunColor = vec4(directionalLight->color.xyz(), context_.scene_->GetDayNightCycle().GetDayNightBlendFactor());
    }
    else
    {
        buffer.sunDirection = vec4(0.f);
        buffer.sunColor     = vec4(0.f);
    }

    context_.graphicsApi_.UpdateShaderBuffer(*bufferId, &buffer);
}
}  // namespace GameEngine
