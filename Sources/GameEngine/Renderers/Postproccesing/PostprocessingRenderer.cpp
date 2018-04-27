#include "PostprocessingRenderer.h"

namespace GameEngine
{
	PostprocessingRenderer::PostprocessingRenderer(RendererContext& context)
		: CRenderer(context.defferedFrameBuffer_.get())
		, rendererContext_(context)
	{

	}
	PostprocessingRenderer::~PostprocessingRenderer()
	{
	}
	void PostprocessingRenderer::Init()
	{
	}
	void PostprocessingRenderer::PrepareFrame(GameEngine::Scene* scene)
	{
	}
	void PostprocessingRenderer::Render(GameEngine::Scene* scene)
	{
	}
	void PostprocessingRenderer::EndFrame(GameEngine::Scene* scene)
	{
	}
	void PostprocessingRenderer::Subscribe(CGameObject* gameObject)
	{
	}
	void PostprocessingRenderer::UnSubscribe(CGameObject* gameObject)
	{
	}
	void PostprocessingRenderer::UnSubscribeAll()
	{
	}
	void PostprocessingRenderer::ReloadShaders()
	{
	}
	void PostprocessingRenderer::CreateRenderers()
	{
	}
}
