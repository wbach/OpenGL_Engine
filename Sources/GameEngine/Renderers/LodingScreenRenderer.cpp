#include "LodingScreenRenderer.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/Loading/LoadingShaderUnfiorms.h"

namespace GameEngine
{
LoadingScreenRenderer::LoadingScreenRenderer(GraphicsApi::IGraphicsApi &api, Texture *bgTexture, Texture *circleTexture,
                                             IShaderFactory &shaderFactory)
    : graphicsApi_(api)
    , circleTexture(circleTexture)
    , backgroundTexture(bgTexture)
{
    shader_ = shaderFactory.create(GraphicsApi::Shaders::Loading);
}

LoadingScreenRenderer::~LoadingScreenRenderer()
{
}

void LoadingScreenRenderer::Init()
{
    shader_->Init();
    CreateBuffers();
}

void LoadingScreenRenderer::Render(Scene *)
{
    shader_->Start();
    prepareRender();
    renderQuad(circleBufferId_, circleTexture->GetId());
    renderQuad(backgroundBufferId_, backgroundTexture->GetId());
    shader_->Stop();
}

void LoadingScreenRenderer::ReloadShaders()
{
}

void LoadingScreenRenderer::prepareRender()
{
    graphicsApi_.PrepareFrame();

    if (circleBufferId_)
    {
        circleBuffer_.TransformationMatrix *= glm::rotate(-1.f, 0.0f, 0.0f, 1.0f);
        graphicsApi_.UpdateShaderBuffer(*circleBufferId_, &circleBuffer_);
    }
}

void LoadingScreenRenderer::renderQuad(const GraphicsApi::ID &bufferId, uint32 textureId) const
{
    if (not bufferId)
        return;

    graphicsApi_.ActiveTexture(0, textureId);
    graphicsApi_.BindShaderBuffer(*bufferId);
    graphicsApi_.RenderQuad();
}
void LoadingScreenRenderer::CreateBuffers()
{
    if (not backgroundBufferId_)
    {
        backgroundBufferId_ = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

        PerObjectUpdate perObjectUpdate;
        perObjectUpdate.TransformationMatrix = mat4(1.f);
        graphicsApi_.UpdateShaderBuffer(*backgroundBufferId_, &perObjectUpdate);
    }

    if (not circleBufferId_)
    {
        circleBufferId_ = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));

        circleBuffer_.TransformationMatrix =
            Utils::CreateTransformationMatrix(vec3(0.81, -0.75, -0.01), vec3(0), vec3(0.1));
        graphicsApi_.UpdateShaderBuffer(*circleBufferId_, &circleBuffer_);
    }
}
}  // namespace GameEngine
