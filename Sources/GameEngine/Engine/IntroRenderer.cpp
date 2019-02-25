#include "IntroRenderer.h"
#include "../Display/DisplayManager.hpp"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
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
    shader_->Start();
    backgroundTexture_ = resorceManager_.GetTextureLaoder().LoadTextureImmediately(
        "GUI/BENGINE.png", false, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);

    if (not perUpdateObjectBuffer_)
    {
        perUpdateObjectBuffer_ =
            graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

        PerObjectUpdate perObjectUpdate;
        perObjectUpdate.TransformationMatrix = glm::transpose(glm::inverse(mat4(1.f)));
        graphicsApi_.UpdateShaderBuffer(*perUpdateObjectBuffer_, &perObjectUpdate);
    }

    initialized_ = true;
}

void IntroRenderer::RenderThis()
{
    graphicsApi_.EnableDepthTest();
    graphicsApi_.PrepareFrame();
    shader_->Start();
    graphicsApi_.BindShaderBuffer(*perUpdateObjectBuffer_);
    renderQuad(mat4(1.f), backgroundTexture_->GetId());
    shader_->Stop();
}

void IntroRenderer::renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const
{
    graphicsApi_.ActiveTexture(0, textureId);
    graphicsApi_.RenderQuad();
}

}  // namespace GameEngine
