#include "TreeRenderer.h"

namespace GameEngine
{
	TreeRenderer::TreeRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, shader(graphicsApi)
		, graphicsApi_(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void TreeRenderer::Init()
	{
	}
	void TreeRenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void TreeRenderer::Render(GameEngine::Scene * scene)
	{
	}
	void TreeRenderer::EndFrame(GameEngine::Scene * scene)
	{
	}
	void TreeRenderer::Subscribe(CGameObject * gameObject)
	{
	}
	void TreeRenderer::UnSubscribe(CGameObject * gameObject)
	{
	}
	void TreeRenderer::UnSubscribeAll()
	{
	}
	void TreeRenderer::ReloadShaders()
	{
	}
	void TreeRenderer::RenderModel(CModel * model, const mat4 & transform_matrix) const
	{
	}
	void TreeRenderer::RenderMesh(const CMesh & mesh, const mat4 & transform_matrix) const
	{
	}
	void TreeRenderer::RenderTrees()
	{
	}
	void TreeRenderer::BindMaterial(const SMaterial & material) const
	{
	}
	void TreeRenderer::UnBindMaterial(const SMaterial & material) const
	{
	}
} // GameEngine
