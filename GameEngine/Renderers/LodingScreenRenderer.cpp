#include "LodingScreenRenderer.h"
#include "../Utils/GLM/GLMUtils.h"
#include "../Resources/Textures/Texture.h"

CLoadingScreenRenderer::CLoadingScreenRenderer(CTexture * bgTexture, CTexture * circleTexture)
	: circleTexture(circleTexture)
	, backgroundTexture(bgTexture)
{
}

void CLoadingScreenRenderer::Init()
{
	quad.Init();
	m_LoadingShader.Init();
	transformation_matrix = Utils::CreateTransformationMatrix(glm::vec3(0.81, -0.75, -0.01), glm::vec3(0), glm::vec3(0.1));
}

void CLoadingScreenRenderer::Render(CScene * scene)
{
	m_LoadingShader.Start();
	// render
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.117f, 0.117f, 0.117f, 1.f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, circleTexture->GetId());
	transformation_matrix *= glm::rotate(-1.f, 0.0f, 0.0f, 1.0f);
	m_LoadingShader.LoadTransformMatrix(transformation_matrix);
	Utils::SimpleRenderVao(quad.vao, quad.m_IndicesSize, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture->GetId());
	m_LoadingShader.LoadTransformMatrix(glm::mat4(1.f));
	Utils::SimpleRenderVao(quad.vao, quad.m_IndicesSize, 2);
	m_LoadingShader.Stop();
}
