#include "IntroRenderer.h"
#include "../Display/DisplayManager.hpp"
#include "GLM/GLMUtils.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/Loading/LoadingShaderUnfiorms.h"

namespace GameEngine
{
IntroRenderer::IntroRenderer(GraphicsApi::IGraphicsApi& graphicsApi, std::shared_ptr<DisplayManager>& displayManager,
                             IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , shaderFactory_(shaderFactory)
    , resorceManager_(graphicsApi)
    , initialized_(false)
{
}
IntroRenderer::~IntroRenderer()
{
}
void IntroRenderer::Render()
{
    if (displayManager_ == nullptr)
        return;

    if (!initialized_)
        Init();

    displayManager_->ProcessEvents();
    RenderThis();
    displayManager_->Update();
}
void IntroRenderer::Init()
{
    shader_ = shaderFactory_.create(GraphicsApi::Shaders::Loading);
    shader_->Init();
    backgroundTexture_ = resorceManager_.GetTextureLaoder().LoadTextureImmediately(
        "GUI/start1.png", false, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);
    initialized_ = true;
}

void IntroRenderer::RenderThis()
{
    shader_->Start();
    graphicsApi_.EnableDepthTest();
    graphicsApi_.PrepareFrame();
    renderQuad(mat4(1.f), backgroundTexture_->GetId());
    shader_->Stop();
}

void IntroRenderer::renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const
{
    graphicsApi_.ActiveTexture(0, textureId);
    shader_->Load(LoadingShaderUniforms::TransformMatrix, transformMatrix);
    graphicsApi_.RenderQuad();
}

}  // namespace GameEngine
