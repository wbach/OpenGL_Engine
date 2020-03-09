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
    displayManager_->UpdateWindow();
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
        perObjectUpdate.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(mat4(1.f));
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
    graphicsApi_.ActiveTexture(0, backgroundTexture_->GetGraphicsObjectId());
    graphicsApi_.RenderQuad();
    shader_->Stop();
}
}  // namespace GameEngine
