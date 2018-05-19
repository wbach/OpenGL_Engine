#pragma once
#include "IRenderer.h"
#include "Objects/Entity/Shaders/SimpleEnityShader.h"
#include "GameEngine/Renderers/RendererFunctionType.h"

class CModel;
class CEntity;
struct SMaterial;
class CProjection;

namespace GameEngine
{
	class SimpleRenderer : public IRenderer
	{
	public:
		SimpleRenderer(IGraphicsApiPtr, CProjection* projection_matrix, std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);

		virtual void Init() override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void ReloadShaders() override;
		void RenderModel(CModel* model, const mat4& transform_matrix) const;

	private:
		void PrepareFrame(Scene* scene);
		void Render(Scene* scene);
		void EndFrame(Scene* scene);
		void BindTextures(const SMaterial& material) const;

	private:
		IGraphicsApiPtr graphicsApi_;
		CSimpleEntityShader shader;
		CProjection* projectionMatrix;

		std::list<CEntity*> subscribes;
	};
} // GameEngine
