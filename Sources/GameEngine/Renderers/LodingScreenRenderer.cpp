#include "LodingScreenRenderer.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
LoadingScreenRenderer::LoadingScreenRenderer(IGraphicsApiPtr api, Texture *bgTexture,
                                             Texture *circleTexture)
    : graphicsApi_(api)
    , loadingShader(api)
    , circleTexture(circleTexture)
    , backgroundTexture(bgTexture)
    , transformationMatrix(1.f)
{
}

void LoadingScreenRenderer::Init()
{
    loadingShader.Init();
    transformationMatrix = Utils::CreateTransformationMatrix(vec3(0.81, -0.75, -0.01), vec3(0), vec3(0.1));
}

void LoadingScreenRenderer::Render(Scene*)
{
    loadingShader.Start();
    prepareRender();
    renderQuad(transformationMatrix, circleTexture->GetId());
    renderQuad(mat4(1.f), backgroundTexture->GetId());
    loadingShader.Stop();
}

void LoadingScreenRenderer::prepareRender()
{
    graphicsApi_->PrepareFrame();
    transformationMatrix *= glm::rotate(-1.f, 0.0f, 0.0f, 1.0f);
}

void LoadingScreenRenderer::renderQuad(const glm::mat4 &transformMatrix, uint32 textureId) const
{
    graphicsApi_->ActiveTexture(0, textureId);
    loadingShader.LoadTransformMatrix(transformMatrix);
    graphicsApi_->RenderQuad();
}
}  // GameEngine
