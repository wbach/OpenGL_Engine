#pragma once
#include "Renderer.h"
#include "../Shaders/Loading/LoadingShader.h"
#include "../Resources/SimpleModels/Quad.h"

class CTexture;

class CLoadingScreenRenderer : public CRenderer
{
public:
	CLoadingScreenRenderer(CTexture* bgTexture, CTexture* circleTexture);
	virtual void Init() override;
    virtual void PrepareFrame(CScene*) override {};
    virtual void Render(CScene*)  override;
    virtual void EndFrame(CScene*) override {};
private:
    CLoadingShader loadingShader;
    SSimpleQuad    quad;
    CTexture*	   circleTexture;
    CTexture*	   backgroundTexture;
    glm::mat4	   transformationMatrix;
};
