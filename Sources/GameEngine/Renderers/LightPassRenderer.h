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
	virtual void PrepareFrame(GameEngine::Scene* scene) override;
	virtual void Render(GameEngine::Scene* scene) override;
	virtual void EndFrame(GameEngine::Scene* scene) override;

private:
    CProjection* projection;
    CLightPassShader shader;
    SSimpleQuad quad;
};
