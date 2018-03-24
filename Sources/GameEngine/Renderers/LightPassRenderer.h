#pragma once
#include "Renderer.h"
#include "GameEngine/Shaders/Deffered/LightPassShader.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CProjection;

class CLightPassRenderer : public CRenderer
{
public:
	CLightPassRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection, CFrameBuffer* frambuffer);
	virtual void Init() override;
	virtual void PrepareFrame(GameEngine::Scene* scene) override;
	virtual void Render(GameEngine::Scene* scene) override;
	virtual void EndFrame(GameEngine::Scene* scene) override;
	virtual void ReloadShaders() override;

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    CProjection* projection;
    CLightPassShader shader;
};
