#include "DefferedLighting.h"
#include "GameEngine/Renderers/Framebuffer/IFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Shaders/DefferedShaderUniforms.h"

namespace GameEngine
{
void DefferedLighting::Init()
{
    shader_.reset(new DefferedShader(rendererContext_->graphicsApi_));
    shader_->Init();

    lightPass_.skyColor     = vec4(0.8, 0.8, 0.8, 1.f);
    lightPass_.viewDistance = rendererContext_->projection_.GetViewDistance();
    lightPass_.screenSize =
        vec2(rendererContext_->projection_.GetWindowSize().x, rendererContext_->projection_.GetWindowSize().y);

    if (not lightPassID_)
    {
        lightPassID_ = rendererContext_->graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION,
                                                          sizeof(DefferedLighting::LightPass));
    }
}
void DefferedLighting::Prepare()
{
}
void DefferedLighting::Render(const Scene& scene)
{
    PrepareApiStateToRender();

    rendererContext_->defferedFrameBuffer_.BindTextures(0);
    shader_->Start();
    LoadLights(scene);
    rendererContext_->graphicsApi_.BindShaderBuffer(*lightPassID_);
    rendererContext_->graphicsApi_.RenderQuad();
    RetriveChanges();
}

void DefferedLighting::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    Init();
}
void DefferedLighting::LoadLights(const Scene& scene)
{
   // if (lightsInGpu_)
     //   return;

    lightPass_.numberOfLights = scene.GetLights().size() + 1;
    convert(scene.GetDirectionalLight(), 0);

    int i = 1;
    for (const auto& light : scene.GetLights())
    {
        convert(light, i++);
    }

    rendererContext_->graphicsApi_.UpdateShaderBuffer(*lightPassID_, &lightPass_);
    lightsInGpu_ = true;
}
void DefferedLighting::PrepareApiStateToRender()
{
    rendererContext_->graphicsApi_.PolygonModeRender();
    rendererContext_->graphicsApi_.DisableDepthMask();
    rendererContext_->graphicsApi_.DisableDepthTest();
    rendererContext_->graphicsApi_.EnableBlend();
}
void DefferedLighting::RetriveChanges()
{
    rendererContext_->graphicsApi_.EnableDepthMask();
    rendererContext_->graphicsApi_.EnableDepthTest();
    rendererContext_->graphicsApi_.DisableBlend();
}
void DefferedLighting::convert(const Light& light, int index)
{
    lightPass_.attenuation_[index] = light.GetAttenuation();
    lightPass_.color_[index]       = light.GetColour();
    lightPass_.cutOff_[index]      = light.GetCutoff();
    lightPass_.position_[index]    = light.GetPosition();
    lightPass_.type_[index]        = static_cast<int>(light.GetType());
}
}  // namespace GameEngine
