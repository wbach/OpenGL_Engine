#pragma once
#include "Renderer.h"
#include "../Shaders/Loading/LoadingShader.h"
#include "GUI/GuiRenderer.h"

class CLoadingScreenRenderer : public CRenderer
{
public:
	CLoadingScreenRenderer(CTexture* bgTexture, CTexture* circleTexture);
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override {};
	virtual void Render(CScene* scene)  override;
	virtual void EndFrame(CScene* scene) override {};
private:
	CLoadingShader m_LoadingShader;
	SSimpleQuad   quad;
	CTexture*	  circleTexture;
	CTexture*	  backgroundTexture;
	glm::mat4	  transformation_matrix;
};