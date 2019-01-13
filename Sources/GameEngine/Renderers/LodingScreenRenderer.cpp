#include "LodingScreenRenderer.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/Loading/LoadingShaderUnfiorms.h"

namespace GameEngine
{
LoadingScreenRenderer::LoadingScreenRenderer(IGraphicsApi& api, Texture *bgTexture, Texture *circleTexture,
                                             IShaderFactory &shaderFactory)
    : graphicsApi_(api)
    , circleTexture(circleTexture)
    , backgroundTexture(bgTexture)
    , transformationMatrix(1.f)
{
    shader_ = shaderFactory.create(Shaders::Loading);
}

LoadingScreenRenderer::~LoadingScreenRenderer()
{
}

void LoadingScreenRenderer::Init()
{
    shader_->Init();
    transformationMatrix = Utils::CreateTransformationMatrix(vec3(0.81, -0.75, -0.01), vec3(0), vec3(0.1));
}

void LoadingScreenRenderer::Render(Scene*)
{
    shader_->Start();
    prepareRender();
    renderQuad(transformationMatrix, circleTexture->GetId());
    renderQuad(mat4(1.f), backgroundTexture->GetId());
    shader_->Stop();
}

void LoadingScreenRenderer::ReloadShaders()
{
}

void LoadingScreenRenderer::prepareRender()
{
    graphicsApi_.PrepareFrame();
    transformationMatrix *= glm::rotate(-1.f, 0.0f, 0.0f, 1.0f);
}

void LoadingScreenRenderer::renderQuad(const glm::mat4 &transformMatrix, uint32 textureId) const
{
    graphicsApi_.ActiveTexture(0, textureId);
    shader_->Load(LoadingShaderUniforms::TransformMatrix, transformMatrix);
    graphicsApi_.RenderQuad();
}
}  // GameEngine
