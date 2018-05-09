#pragma once
#include "Shaders/ParticlesShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/Renderer.h"
#include "Shaders/AnimatedParticlesShader.h"
#include "GameEngine/Objects/RenderAble/Paricle/Particle.h"

class CProjection;
class CTexture;

namespace GameEngine
{
	struct ParticleSubscriber
	{
		bool isAnimated = false;
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
		void InitShaders();
		void StartShader();
		void StopShader();
		void PrepareFrame();
		void ClearFrame();
		void RenderSubscribes(const mat4& viewMatrix);
		void RenderParticles(const ParticleSubscriber& effect, const mat4& viewMatrix);
		void RenderInstances(uint32 size);
		mat4 UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix);
		void UpdateTexture(CTexture* texture);
		void ReallocationParticlesData(uint32 size);
		void GetParticleData(const std::vector<Particle>& particles, const mat4& viewMatrix);
		vec4 GetTextureOffsets(float blendFactor);

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		ParticlesShader shader;
		AnimatedParticlesShader animatedShader_;
		CProjection* projectionMatrix;
		SubscribesMap subscribers_;
		uint32 particleObjecId;
		uint32 aniamtedParticleObjecId;
		uint32 staticParticleObjecId;

	private:
		bool currentUseAnimation;
		uint32 textureNumberOfrows;
		std::vector<vec4> offsets_;
		std::vector<float> blendFactors_;
		std::vector<mat4> transformsParticles_;
	};
} // GameEngine
