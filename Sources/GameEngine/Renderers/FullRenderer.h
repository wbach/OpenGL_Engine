#pragma once
#include "Renderer.h"
#include "RendererContext.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <memory>
#include <vector>
#include <unordered_map>

class CProjection;

class FullRenderer : public CRenderer
{
public:
    FullRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix);
	~FullRenderer();
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene* scene) override;
    virtual void Render(GameEngine::Scene* scene) override;
    virtual void EndFrame(GameEngine::Scene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;
	virtual void UnSubscribe(CGameObject* gameObject) override;
	virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
	void CreateRenderers();

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;

    // ShadowMap renderes, etc...
    std::vector<std::unique_ptr<CRenderer>> renderers;
   
	GameEngine::RendererContext rendererContext_;
};
