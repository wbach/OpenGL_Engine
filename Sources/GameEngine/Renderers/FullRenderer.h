#pragma once
#include "RendererContext.h"
#include "Renderer.h"
#include <memory>
#include <vector>


class CDefferedFrameBuffer;
class CProjection;

class FullRenderer : public CRenderer
{
public:
    FullRenderer(CProjection* projection_matrix);
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
    CProjection* projectionMatrix;

    // ShadowMap renderes, etc...
    std::vector<std::unique_ptr<CRenderer>> renderers;
    std::shared_ptr<CDefferedFrameBuffer> defferedFrameBuffer;
   
	GameEngine::RendererContext rendererContext_;
};
