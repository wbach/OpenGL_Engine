#pragma once
#include "GameEngine/Renderers/Renderer.h"
#include "Shaders/TerrainShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <list>

class CModel;
class CTexture;
class CProjection;
class CTerrainWrapper;
typedef CTerrainWrapper* TerrainPtr;
typedef std::vector<TerrainPtr> TerrainPtrs;

namespace GameEngine
{
	struct RendererContext;

	class CTerrainRenderer : public CRenderer
	{
	public:
		CTerrainRenderer(IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer, RendererContext* shadowRendererContext);
		// Loading lights itp to shader
		virtual void Init() override;
		virtual void PrepareFrame(GameEngine::Scene* scene) override;
		virtual void Render(GameEngine::Scene* scene) override;
		virtual void EndFrame(GameEngine::Scene* scene) override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void ReloadShaders() override;

	private:
		void BindTexture(CTexture* texture, int id) const;
		void BindTextures(TerrainPtr terrain) const;
		void RenderSubscribers(const mat4& viewMatrix, int range) const;
		void RenderSubscriber(TerrainPtr) const;
		void InitShader();

	private:
		IGraphicsApiPtr graphicsApi_;
		TerrainShader  shader;
		CProjection*	projectionMatrix;
		RendererContext* rendererContext_;

		vec4	clipPlane;

		TerrainPtrs subscribes;

		uint32 objectId;
	};

}

