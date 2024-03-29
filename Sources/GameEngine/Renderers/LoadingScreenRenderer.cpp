#include "LoadingScreenRenderer.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
LoadingScreenRenderer::LoadingScreenRenderer(GraphicsApi::IGraphicsApi &api, Texture& bgTexture, Texture& circleTexture)
    : graphicsApi_(api)
    , shader_(api, GraphicsApi::ShaderProgramType::Loading)
    , circleTexture(circleTexture)
    , backgroundTexture(bgTexture)
    , circleMatrix_(1.f)
    , timer_(0, 16)
{
}

LoadingScreenRenderer::~LoadingScreenRenderer()
{
}

void LoadingScreenRenderer::init()
{
    shader_.Init();
    CreateBuffers();
    const auto windowSize = graphicsApi_.GetWindowApi().GetWindowSize();
    graphicsApi_.SetViewPort(0, 0, windowSize.x, windowSize.y);

    timer_.AddOnTickCallback([this]()
    {
        circleMatrix_ *= glm::rotate(glm::radians(-1.f), glm::vec3(0.f, 0.f, 1.f));
    });
}

void LoadingScreenRenderer::render()
{
    timer_.StartFrame();
    prepareRender();
    shader_.Start();
    graphicsApi_.EnableBlend();
    graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA);
    renderQuad(backgroundBufferId_, backgroundTexture.GetGraphicsObjectId());
    renderQuad(circleBufferId_, circleTexture.GetGraphicsObjectId());
    graphicsApi_.DisableBlend();
    shader_.Stop();
    timer_.EndFrame();
}

void LoadingScreenRenderer::reloadShaders()
{
    shader_.Reload();
}

void LoadingScreenRenderer::prepareRender()
{
    graphicsApi_.PrepareFrame();
    if (circleBufferId_)
    {
        PerObjectUpdate perObjectUpdate_;
        perObjectUpdate_.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(circleMatrix_);
        graphicsApi_.UpdateShaderBuffer(*circleBufferId_, &perObjectUpdate_);
    }
}

void LoadingScreenRenderer::renderQuad(const GraphicsApi::ID &bufferId, const GraphicsApi::ID &textureId) const
{
    if (not bufferId)
        return;

    if (textureId)
        graphicsApi_.ActiveTexture(0, *textureId);

    graphicsApi_.BindShaderBuffer(*bufferId);
    graphicsApi_.RenderQuad();
}
void LoadingScreenRenderer::CreateBuffers()
{
    if (not backgroundBufferId_)
    {
        backgroundBufferId_ = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

        PerObjectUpdate perObjectUpdate_;
        perObjectUpdate_.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(mat4(1.f));
        graphicsApi_.UpdateShaderBuffer(*backgroundBufferId_, &perObjectUpdate_);
    }

    if (not circleBufferId_)
    {
        circleBufferId_ = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
        circleMatrix_ = Utils::CreateTransformationMatrix(vec3(0.81, -0.75, -0.1), DegreesVec3(0), vec3(0.1f));
       
        PerObjectUpdate perObjectUpdate_;
        perObjectUpdate_.TransformationMatrix = graphicsApi_.PrepareMatrixToLoad(circleMatrix_);
        graphicsApi_.UpdateShaderBuffer(*circleBufferId_, &perObjectUpdate_);
    }
}
}  // namespace GameEngine
