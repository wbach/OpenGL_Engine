#include "DefferedLighting.h"

#include "GameEngine/Components/Lights/DirectionalLightComponent.h"
#include "GameEngine/Components/Lights/PointLightComponent.h"
#include "GameEngine/Components/Lights/SpotLightComponent.h"
#include "GameEngine/Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
DefferedLighting::DefferedLighting(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Deffered)
{
}
void DefferedLighting::Init()
{
    shader_.Init();

    if (not lightPassID_)
    {
        lightPassID_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
            PER_MESH_OBJECT_BIND_LOCATION, sizeof(DefferedLighting::LightPass), GraphicsApi::DrawFlag::Dynamic);
    }

    if (not directionalLightsId_)
    {
        directionalLightsId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
            DIRECTIONAL_LIGHTS_BIND_LOCATION, sizeof(DefferedLighting::DirectionalLights), GraphicsApi::DrawFlag::Dynamic);
    }

    if (not pointLightsId_)
    {
        pointLightsId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
            POINT_LIGHTS_BIND_LOCATION, sizeof(DefferedLighting::PointlLights), GraphicsApi::DrawFlag::Dynamic);
    }

    if (not spotLightsId_)
    {
        spotLightsId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(
            SPOT_LIGHTS_BIND_LOCATION, sizeof(DefferedLighting::SpotLights), GraphicsApi::DrawFlag::Dynamic);
    }
}
void DefferedLighting::CleanUp()
{
    if (not lightPassID_)
    {
        rendererContext_.graphicsApi_.DeleteShaderBuffer(*lightPassID_);
        lightPassID_.reset();
    }

    if (not directionalLightsId_)
    {
        rendererContext_.graphicsApi_.DeleteShaderBuffer(*directionalLightsId_);
        directionalLightsId_.reset();
    }

    if (not pointLightsId_)
    {
        rendererContext_.graphicsApi_.DeleteShaderBuffer(*pointLightsId_);
        pointLightsId_.reset();
    }

    if (not spotLightsId_)
    {
        rendererContext_.graphicsApi_.DeleteShaderBuffer(*spotLightsId_);
        spotLightsId_.reset();
    }
}

void DefferedLighting::Prepare()
{
}
void DefferedLighting::Render(const Scene& scene)
{
    PrepareApiStateToRender();
    shader_.Start();
    LoadLights(scene);
    rendererContext_.graphicsApi_.BindShaderBuffer(*lightPassID_);
    rendererContext_.graphicsApi_.BindShaderBuffer(*directionalLightsId_);
    rendererContext_.graphicsApi_.BindShaderBuffer(*pointLightsId_);
    rendererContext_.graphicsApi_.BindShaderBuffer(*spotLightsId_);

    const uint32 uniformSkyTextureIndex = 5;
    auto skyTextureId = rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::skyTexture).value()];
    if (skyTextureId)
    {
        rendererContext_.graphicsApi_.ActiveTexture(uniformSkyTextureIndex, *skyTextureId);
    }
    const uint32 uniformLightshaftTextureIndex = 6;
    auto lightShaftTextureId = rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::lightShafts).value()];
    if (lightShaftTextureId)
    {
        rendererContext_.graphicsApi_.ActiveTexture(uniformLightshaftTextureIndex, *lightShaftTextureId);
    }

    bindShadowMapCascades();

    rendererContext_.graphicsApi_.RenderQuad();
    RetriveChanges();
}

void DefferedLighting::bindShadowMapCascades() const
{
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        const uint32 uniformShadowMapIndex = 7;
        rendererContext_.graphicsApi_.ActiveTexture(uniformShadowMapIndex + cascadeIndex);
        auto shadomapIndex = magic_enum::enum_index(SharedTextures::shadowCascade0).value() + cascadeIndex;
        auto shadowmapId   = rendererContext_.sharedTextures[shadomapIndex];
        if (shadowmapId)
        {
            rendererContext_.graphicsApi_.BindTexture(*shadowmapId);
        }
    }
}

void DefferedLighting::ReloadShaders()
{
    shader_.Reload();
}
void DefferedLighting::LoadLights(const Scene& scene)
{
    if (not rendererContext_.camera_)
    {
        LOG_ERROR << "Camera not set!";
        return;
    }

    const auto& projection  = rendererContext_.camera_->GetProjection();
    lightPass_.skyColor     = vec4(rendererContext_.fogColor_, 1.f);
    lightPass_.viewDistance = projection.GetViewDistance();
    lightPass_.screenSize   = vec2(projection.GetRenderingSize().x, projection.GetRenderingSize().y);

    auto& cc                         = scene.getComponentController();
    auto directionalLightsComponents = cc.GetAllActiveComponentsOfType<Components::DirectionalLightComponent>();
    auto pointLightsComponents       = cc.GetAllActiveComponentsOfType<Components::PointLightComponent>();
    auto spotLightsComponents        = cc.GetAllActiveComponentsOfType<Components::SpotLightComponent>();

    lightPass_.numberOfLights =
        vec3ui(directionalLightsComponents.size(), pointLightsComponents.size(), spotLightsComponents.size());

    FillDirectionalLightsBuffer(directionalLightsComponents);
    FillPointLightsBuffer(pointLightsComponents);
    FillSpotLightsBuffer(spotLightsComponents);

    rendererContext_.graphicsApi_.UpdateShaderBuffer(*lightPassID_, &lightPass_);
    rendererContext_.graphicsApi_.UpdateShaderBuffer(*directionalLightsId_, &directionalLights_);
    rendererContext_.graphicsApi_.UpdateShaderBuffer(*pointLightsId_, &pointLights_);
    rendererContext_.graphicsApi_.UpdateShaderBuffer(*spotLightsId_, &spotLights_);

    lightsInGpu_ = true;
}
void DefferedLighting::PrepareApiStateToRender()
{
    rendererContext_.graphicsApi_.PolygonModeRender();
    rendererContext_.graphicsApi_.DisableDepthMask();
    rendererContext_.graphicsApi_.DisableDepthTest();
    //  rendererContext_.graphicsApi_.EnableBlend();
}
void DefferedLighting::RetriveChanges()
{
    rendererContext_.graphicsApi_.EnableDepthMask();
    rendererContext_.graphicsApi_.EnableDepthTest();
    // rendererContext_.graphicsApi_.DisableBlend();
}
void DefferedLighting::FillDirectionalLightsBuffer(const std::vector<Components::DirectionalLightComponent*>& directionalLights)
{
    size_t count = std::min(directionalLights.size(), static_cast<size_t>(MAX_DIR_LIGHTS));
    for (size_t i = 0; i < count; ++i)
    {
        directionalLights_.direction[i] = directionalLights[i]->GetDirection();
        directionalLights_.color[i]     = vec4(directionalLights[i]->color.xyz(), directionalLights[i]->intensity);
    }
    // Fill remaining slots with zero
    for (size_t i = count; i < MAX_DIR_LIGHTS; ++i)
    {
        directionalLights_.direction[i] = vec3(0.0f);
        directionalLights_.color[i]     = vec4(0.0f);
    }
}
void DefferedLighting::FillPointLightsBuffer(const std::vector<Components::PointLightComponent*>& pointLights)
{
    size_t count = std::min(pointLights.size(), static_cast<size_t>(MAX_POINT_LIGHTS));
    for (size_t i = 0; i < count; ++i)
    {
        pointLights_.color[i]  = vec4(pointLights[i]->color.xyz(), pointLights[i]->intensity);
        pointLights_.params[i] = vec4(pointLights[i]->intensity, pointLights[i]->range, pointLights[i]->falloffExponent, 0.0f);
        pointLights_.worldPosition[i] = vec4(pointLights[i]->GetParentGameObject().GetWorldTransform().GetPosition(), 0.f);
    }
    for (size_t i = count; i < MAX_POINT_LIGHTS; ++i)
    {
        pointLights_.color[i]         = vec4(0.0f);
        pointLights_.params[i]        = vec4(0.0f);
        pointLights_.worldPosition[i] = vec4(0.0f);
    }
}
void DefferedLighting::FillSpotLightsBuffer(const std::vector<Components::SpotLightComponent*>& spotLights)
{
    size_t count = std::min(spotLights.size(), static_cast<size_t>(MAX_SPOT_LIGHTS));

    for (size_t i = 0; i < count; ++i)
    {
        spotLights_.color[i]         = vec4(spotLights[i]->color.xyz(), spotLights[i]->intensity);
        spotLights_.params[i]        = vec4(spotLights[i]->intensity, spotLights[i]->range, spotLights[i]->falloffExponent, 0.0f);
        spotLights_.direction[i]     = spotLights[i]->direction;
        spotLights_.cutoff[i]        = vec4(spotLights[i]->innerCutoff, spotLights[i]->outerCutoff, 0.0f, 0.0f);
        spotLights_.worldPosition[i] = vec4(spotLights[i]->GetParentGameObject().GetWorldTransform().GetPosition(), 0.f);
    }
    for (size_t i = count; i < MAX_SPOT_LIGHTS; ++i)
    {
        spotLights_.color[i]         = vec4(0.0f);
        spotLights_.params[i]        = vec4(0.0f);
        spotLights_.direction[i]     = vec3(0.0f);
        spotLights_.cutoff[i]        = vec4(0.0f);
        spotLights_.worldPosition[i] = vec4(0.0f);
    }
}
}  // namespace GameEngine
