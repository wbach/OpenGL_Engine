#pragma once
#include "../Renderer.h"
#include "GameEngine/Objects/RenderAble/Paricle/Particle.h"
#include "Shaders/ParticlesShader.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CProjection;
class CTexture;

namespace GameEngine
{
	struct ParticleSubscriber
	{
		std::vector<Particle>* particles = nullptr;
		CTexture* texture;
	};
	class ParticlesRenderer : public CRenderer
	{
	public:
		ParticlesRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer);
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
		void UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix);

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		ParticlesShader shader;
		CProjection* projectionMatrix;
		std::unordered_map<uint32, ParticleSubscriber> subscribers_;
	};
} // GameEngine
