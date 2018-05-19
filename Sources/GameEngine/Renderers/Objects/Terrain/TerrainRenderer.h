#pragma once
#include "Shaders/TerrainShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"

class CModel;
class CTexture;
class CProjection;
class CTerrainWrapper;
typedef CTerrainWrapper* TerrainPtr;
typedef std::vector<TerrainPtr> TerrainPtrs;

namespace GameEngine
{
	struct RendererContext;

	class TerrainRenderer : public IRenderer
	{
	public:
		TerrainRenderer(RendererContext& context);
		// Loading lights itp to shader
		virtual void Init() override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void ReloadShaders() override;
		void Render(Scene* scene);

	private:
		void BindTexture(CTexture* texture, int id) const;
		void BindTextures(TerrainPtr terrain) const;
		void RenderSubscribers(const mat4& viewMatrix, int range) const;
		void RenderSubscriber(TerrainPtr) const;
		void InitShader();

	private:
		RendererContext& context_;
		TerrainShader  shader;
		vec4	clipPlane;

		TerrainPtrs subscribes;

		uint32 objectId;
	};

}

