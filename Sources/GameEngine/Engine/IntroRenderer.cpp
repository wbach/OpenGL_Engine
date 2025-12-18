#include "IntroRenderer.h"

#include <Logger/Log.h>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Resources/DefaultFiles/bengineLogo.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/IResourceManagerFactory.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
IntroRenderer::IntroRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader,
                             DisplayManager& displayManager, IResourceManagerFactory& resourceManagerFactory)
    : graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , resourceManager_(resourceManagerFactory.create())
    , shader_(graphicsApi, GraphicsApi::ShaderProgramType::Loading)
    , initialized_(false)
{
}
IntroRenderer::~IntroRenderer()
{
    LOG_DEBUG << "destructor";
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

    TextureParameters params;
    params.loadType        = TextureLoadType::Immediately;
    params.flipMode        = TextureFlip::VERTICAL;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;

    backgroundTexture_ = resourceManager_->GetTextureLoader().LoadTexture("BENGINE_LOGO", BENGINE_png, BENGINE_png_len, params);

    if (not perUpdateObjectBuffer_)
    {
        perUpdateObjectBuffer_ = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate),
                                                                 GraphicsApi::DrawFlag::Static);

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

    if (backgroundTexture_ and backgroundTexture_->GetGraphicsObjectId())
        graphicsApi_.ActiveTexture(0, *backgroundTexture_->GetGraphicsObjectId());

    graphicsApi_.RenderQuad();
    shader_.Stop();
}
}  // namespace GameEngine
