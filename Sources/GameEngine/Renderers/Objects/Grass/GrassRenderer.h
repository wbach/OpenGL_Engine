#pragma once
#include "Shaders/GrassShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"

struct SGrass;
class CMesh;
class CModel;
class CProjection;

namespace GameEngine
{
	class GrassRenderer : public IRenderer
	{
	public:
		GrassRenderer(RendererContext& context);
		virtual void Init() override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void ReloadShaders() override;
		void Render(Scene* scene);

	private:
		void InitShader();
		void PrepareRender(Scene* scene);
		void EndRender() const;
		void RenderSubscribes();
		void RenderModel(CModel* model);
		void RenderMesh(const CMesh& mesh);
		void PrepareShader(Scene* scene);

	private:
		RendererContext& context_;
		CGrassShader shader;

		std::list<SGrass*> subscribes;
		float viewDistance = 30.f;
	};
} // GameEngine
