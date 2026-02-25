#include "IntroRenderer.h"

#include <Logger/Log.h>
#include <unistd.h>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Resources/DefaultFiles/bengineLogo.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
IntroRenderer::IntroRenderer(GraphicsApi::IGraphicsApi& graphicsApi, DisplayManager& displayManager)
    : graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , shader_(graphicsApi, GraphicsApi::ShaderProgramType::Loading)
{
}
IntroRenderer::~IntroRenderer()
{
    LOG_DEBUG << "destructor";
}

void IntroRenderer::Render()
{
    Init();

    displayManager_.ProcessEvents();
    RenderThis();
    displayManager_.UpdateWindow();

    CleanUp();

    //sleep(2);
}
void IntroRenderer::Init()
{
    shader_.Init();
    shader_.Start();

    TextureParameters params;
    params.loadType        = TextureLoadType::Immediately;
    params.flipMode        = TextureFlip::VERTICAL;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;

    auto image = ReadImage(BENGINE_png, BENGINE_png_len, params);

    if (not image)
    {
        LOG_DEBUG << "Something goes wrong!";
        return;
    }

    backgroundTexture_ = std::make_unique<GeneralTexture>(graphicsApi_, std::move(*image), params);
    backgroundTexture_->GpuLoadingPass();

    if (not perUpdateObjectBuffer_)
    {
        perUpdateObjectBuffer_ = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate),
                                                                 GraphicsApi::DrawFlag::Static);

        PerObjectUpdate perObjectUpdate;
        perObjectUpdate.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(mat4(1.f));
        graphicsApi_.UpdateShaderBuffer(*perUpdateObjectBuffer_, &perObjectUpdate);
    }
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
void IntroRenderer::CleanUp()
{
    shader_.Clear();

    if (backgroundTexture_)
    {
        backgroundTexture_->ReleaseGpuPass();
        backgroundTexture_.reset();
    }

    if (perUpdateObjectBuffer_)
    {
        graphicsApi_.DeleteShaderBuffer(*perUpdateObjectBuffer_);
        perUpdateObjectBuffer_.reset();
    }
}
}  // namespace GameEngine
