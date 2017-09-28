#include "LodingScreenRenderer.h"
#include "../Resources/Textures/Texture.h"
#include "GLM/GLMUtils.h"

CLoadingScreenRenderer::CLoadingScreenRenderer(CTexture * bgTexture, CTexture * circleTexture)
	: circleTexture(circleTexture)
	, backgroundTexture(bgTexture)
{
}

void CLoadingScreenRenderer::Init()
{
	quad.Init();
	loadingShader.Init();
	transformationMatrix = Utils::CreateTransformationMatrix(vec3(0.81, -0.75, -0.01), vec3(0), vec3(0.1));
}

void CLoadingScreenRenderer::Render(CScene * scene)
{
	loadingShader.Start();
	prepareRender();
	renderQuad(transformationMatrix, circleTexture->GetId());
	renderQuad(mat4(1.f), backgroundTexture->GetId());
	loadingShader.Stop();
}

void CLoadingScreenRenderer::prepareRender()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.117f, 0.117f, .117f, 1.f);
	transformationMatrix *= glm::rotate(-1.f, 0.0f, 0.0f, 1.0f);
}

void CLoadingScreenRenderer::renderQuad(const glm::mat4 & transformMatrix, uint32 textureId) const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	loadingShader.LoadTransformMatrix(transformMatrix);
	Utils::SimpleRenderVao(quad.vao, quad.indicesSize, 2);
}
