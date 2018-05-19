#pragma once
#include "IRenderer.h"
#include "RendererContext.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Postproccesing/PostprocessingRenderersManager.h"
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

class CProjection;

namespace GameEngine
{
typedef std::unique_ptr<IRenderer> RendererPtr;
typedef std::vector<RendererPtr> RendererVecPtr;

class FullRenderer : public IRenderer
{
public:
    FullRenderer(IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
	~FullRenderer();
    // Loading lights itp to shader
	virtual void Init() override;
    virtual void Subscribe(CGameObject* gameObject) override;
	virtual void UnSubscribe(CGameObject* gameObject) override;
	virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
	void CreateRenderers();
	void PostProcess(Scene*);
	void Prepare(Scene*);
	template <class T>
	void AddRenderer();

private:
	RendererContext context_;
    RendererVecPtr renderers;
	PostProcessingManager postprocessingRenderersManager_;
};
} // GameEngine
