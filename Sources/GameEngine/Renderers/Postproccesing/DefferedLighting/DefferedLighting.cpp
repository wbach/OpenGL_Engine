#include "DefferedLighting.h"
#include "GameEngine/Renderers/Framebuffer/IFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
void DefferedLighting::Init()
{
    shader_.reset(new DefferedShader(rendererContext_->graphicsApi_));
    shader_->Init();
    shader_->Start();
    shader_->Load(DefferedShader::SkyColour, vec3(0.8));
    shader_->Load(DefferedShader::ScreenSize, rendererContext_->projection_->GetWindowSize());
    shader_->Load(DefferedShader::ViewDistance, rendererContext_->projection_->GetViewDistance());
    shader_->Stop();
}

void DefferedLighting::Prepare(Scene*)
{
}

void DefferedLighting::Render(Scene* scene)
{
    rendererContext_->graphicsApi_->PolygonModeRender();
    rendererContext_->graphicsApi_->DisableDepthMask();
    rendererContext_->graphicsApi_->DisableDepthTest();
    rendererContext_->graphicsApi_->EnableBlend();

    if (rendererContext_->defferedFrameBuffer_ == nullptr)
        return;

    rendererContext_->defferedFrameBuffer_->BindTextures(0);
    // rendererContext_->defferedFrameBuffer_->UnBind();

    shader_->Start();
    shader_->Load(DefferedShader::CameraPosition, scene->GetCamera()->GetPosition());
    shader_->LoadLight(0, scene->GetDirectionalLight());
    int lights = scene->GetLights().size() + 1;
    shader_->Load(DefferedShader::NumberOfLights, lights);
    int i = 1;
    for (const auto& light : scene->GetLights())
    {
        shader_->LoadLight(i++, light);
    }
    rendererContext_->graphicsApi_->RenderQuad();
    shader_->Stop();
}

void DefferedLighting::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    Init();
}
}  // GameEngine
