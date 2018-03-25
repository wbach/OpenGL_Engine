#pragma once
#include "../Renderer.h"
#include "Shaders/TreeShader.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CProjection;
struct SMaterial;
class CEntity;
class CModel;
class CMesh;

namespace GameEngine
{
	class ModelWrapper;

	struct Subscriber
	{
		std::vector<vec3>* positions;
		GameEngine::ModelWrapper* top;
		GameEngine::ModelWrapper* bottom;
		mat4 transform;
		vec2ui textureSize;
		bool textureInGpu = false;
		uint32 positionTexture;
	};

	class TreeRenderer : public CRenderer
	{
	typedef std::unordered_map<uint32_t, Subscriber> SubscribersMap;
	public:
		TreeRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer);
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
		void PreparePositionMap(Subscriber& sub);
		void RenderModel(CModel* model, const mat4&) const;
		void RenderMesh(const CMesh& mesh, const mat4&) const;
		void RenderTrees();
		void BindMaterial(const SMaterial& material) const;
		void UnBindMaterial(const SMaterial& material) const;

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		TreeShader shader;
		CProjection* projectionMatrix;

		vec4 clipPlane;

		SubscribersMap subscribes_;
	};
} // GameEngine
