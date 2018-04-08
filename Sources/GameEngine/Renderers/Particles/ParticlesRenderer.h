#pragma once
#include "../Renderer.h"
#include "GameEngine/Objects/RenderAble/Paricle/Particle.h"
#include "Shaders/ParticlesShader.h"
#include "Shaders/AnimatedParticlesShader.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CProjection;
class CTexture;

namespace GameEngine
{
	struct ParticleSubscriber
	{
		CTexture* texture = nullptr;
		BlendFunctionType blendFunction;
		std::vector<Particle>* particles = nullptr;
	};

	class ParticlesRenderer : public CRenderer
	{
	typedef std::unordered_map<uint32, ParticleSubscriber> SubscribesMap;
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
		void PrepareFrame();
		void ClearFrame();
		void RenderSubscribes(const mat4& viewMatrix);
		void RenderAnimatedSubscribes(const mat4& viewMatrix);
		mat4 UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix);
		void RenderParticles(const std::vector<Particle>& particles, const mat4& viewMatrix);

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		ParticlesShader shader;
		AnimatedParticlesShader animatedShader_;
		CProjection* projectionMatrix;
		SubscribesMap subscribers_;
		SubscribesMap animatedSubscribers_;
	};
} // GameEngine
