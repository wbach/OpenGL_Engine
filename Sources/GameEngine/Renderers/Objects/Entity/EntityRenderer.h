#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "Shaders/GeometryEntityShader.h"
#include "GameEngine/Renderers/IRenderer.h"

class CProjection;
class CEntity;
class CModel;
class CMesh;

namespace GameEngine
{
	class ModelWrapper;
	struct RendererContext;

	struct EntitySubscriber
	{
		uint32 textureIndex;
		CGameObject* gameObject;
		ModelWrapper* model;
	};

	typedef std::unordered_map<uint32_t, EntitySubscriber> SubscribersMap;

	class EntityRenderer : public IRenderer
	{
	public:
		EntityRenderer(RendererContext& context);
		// Loading lights itp to shader
		virtual void Init() override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void UnSubscribeAll() override;
		virtual void ReloadShaders() override;
		void Render(Scene* scene);

	private:
		void InitShader();
		void PrepareFrame(Scene* scene);
		void RenderModel(CModel* model, const mat4& transform_matrix) const;
		void RenderMesh(const CMesh& mesh, const mat4& transform_matrix) const;
		void RenderEntities();
		void BindMaterial(const SMaterial& material) const;
		void UnBindMaterial(const SMaterial& material) const;

	private:
		RendererContext& context_;
		CEntityGeometryPassShader shader;
		vec4 clipPlane;

		uint32 currentTextureIndex_ = 0;
		SubscribersMap subscribes_;
	};

} // GameEngine
