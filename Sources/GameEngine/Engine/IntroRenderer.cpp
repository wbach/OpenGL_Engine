#include "IntroRenderer.h"

#include "../Display/DisplayManager.hpp"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include <Logger/Log.h>

namespace GameEngine
{
IntroRenderer::IntroRenderer(GraphicsApi::IGraphicsApi& graphicsApi, DisplayManager& displayManager)
    : graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , resourceManager(graphicsApi)
    , shader_(graphicsApi, GraphicsApi::ShaderProgramType::Loading)
    , initialized_(false)
{
}
IntroRenderer::~IntroRenderer()
{
    DEBUG_LOG("destructor");
}

void IntroRenderer::Render()
{
    if (not initialized_)
        Init();

    displayManager_.ProcessEvents();
    RenderThis();
    displayManager_.UpdateWindow();
}
void IntroRenderer::Init()
{
    shader_.Init();
    shader_.Start();
    backgroundTexture_ = resourceManager.GetTextureLaoder().LoadTextureImmediately(
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
    shader_.Start();
    graphicsApi_.BindShaderBuffer(*perUpdateObjectBuffer_);
    graphicsApi_.ActiveTexture(0, backgroundTexture_->GetGraphicsObjectId());
    graphicsApi_.RenderQuad();
    shader_.Stop();
}
}  // namespace GameEngine
