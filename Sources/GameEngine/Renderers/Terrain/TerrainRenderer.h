#pragma once
#include "GameEngine/Renderers/Renderer.h"
#include "Shaders/TerrainShader.h"
#include <list>

class CModel;
class CTexture;
class CProjection;
class CTerrainWrapper;
class CShadowFrameBuffer;
typedef CTerrainWrapper* TerrainPtr;
typedef std::vector<TerrainPtr> TerrainPtrs;

namespace GameEngine
{
	class CTerrainRenderer : public CRenderer
	{
	public:
		CTerrainRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer, CShadowFrameBuffer* shadowFramebuffer);
		// Loading lights itp to shader
		virtual void Init() override;
		virtual void PrepareFrame(GameEngine::Scene* scene) override;
		virtual void Render(GameEngine::Scene* scene) override;
		virtual void EndFrame(GameEngine::Scene* scene) override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;

	private:
		void BindTexture(CTexture* texture, int id) const;
		void BindTextures(TerrainPtr terrain) const;
		void RenderSubscribers(const vec3 & camera_position, int range) const;
		void RenderSubscriber(TerrainPtr) const;

	private:
		TerrainShader  shader;
		CProjection*	projectionMatrix;
		CShadowFrameBuffer* shadowFramebuffer;

		vec4	clipPlane;

		TerrainPtrs subscribes;

		GLuint vao;
	};

}

