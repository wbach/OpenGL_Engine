#pragma once
#include "Renderer.h"
#include "../Resources/SimpleModels/Quad.h"
#include "../Shaders/Deffered/LightPassShader.h"

class CProjection;

class CLightPassRenderer : public CRenderer
{
public:
	CLightPassRenderer(CProjection* projection, CFrameBuffer* frambuffer);
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;

private:
    CProjection* projection;
    CLightPassShader shader;
    SSimpleQuad quad;
};
