#include "LodingScreenRenderer.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GLM/GLMUtils.h"

CLoadingScreenRenderer::CLoadingScreenRenderer(GameEngine::IGraphicsApiPtr api, CTexture * bgTexture, CTexture * circleTexture)
	: graphicsApi_(api)
	, circleTexture(circleTexture)
	, backgroundTexture(bgTexture)
	, loadingShader(api)
{
}

void CLoadingScreenRenderer::Init()
{
	loadingShader.Init();
	transformationMatrix = Utils::CreateTransformationMatrix(vec3(0.81, -0.75, -0.01), vec3(0), vec3(0.1));
}

void CLoadingScreenRenderer::Render(GameEngine::Scene* scene)
{
	loadingShader.Start();
	prepareRender();
	renderQuad(transformationMatrix, circleTexture->GetId());
	renderQuad(mat4(1.f), backgroundTexture->GetId());
	loadingShader.Stop();
}

void CLoadingScreenRenderer::prepareRender()
{
	graphicsApi_->PrepareFrame();
	transformationMatrix *= glm::rotate(-1.f, 0.0f, 0.0f, 1.0f);
}

void CLoadingScreenRenderer::renderQuad(const glm::mat4 & transformMatrix, uint32 textureId) const
{
	graphicsApi_->ActiveTexture(0, textureId);
	loadingShader.LoadTransformMatrix(transformMatrix);
	graphicsApi_->RenderQuad();
}
