#include "ParticlesRenderer.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"
#include "../Framebuffer/FrameBuffer.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	float kwadrat(float blend)
	{
		return -1.f * blend * blend + 2 * blend;
	}
	float wielomian(float blend)
	{
		auto r = -3.2f * blend * blend *blend *blend + 8.8f *blend *blend *blend - 9.2f * blend * blend + 4.6f * blend - 4e-12;
		if (r < 0.f)
			return 0.f;
		if (r > 1.f)
			return 1.f;
		return r;
	}
	ParticlesRenderer::ParticlesRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, graphicsApi_(graphicsApi)
		, shader(graphicsApi)
		, animatedShader_(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void ParticlesRenderer::Init()
	{
		shader.Init();
		shader.Start();
		shader.Load(ParticlesShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());
		shader.Stop();

		animatedShader_.Init();
		animatedShader_.Start();
		animatedShader_.Load(AnimatedParticlesShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());
		animatedShader_.Stop();
	}
	void ParticlesRenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void ParticlesRenderer::Render(GameEngine::Scene * scene)
	{
		PrepareFrame();
		
		shader.Start();
		RenderSubscribes(scene->GetCamera()->GetViewMatrix());
		shader.Stop();

		animatedShader_.Start();
		RenderAnimatedSubscribes(scene->GetCamera()->GetViewMatrix());
		animatedShader_.Stop();
		ClearFrame();
	}
	void ParticlesRenderer::EndFrame(GameEngine::Scene * scene)
	{
	}
	void ParticlesRenderer::Subscribe(CGameObject* gameObject)
	{
		if (gameObject == nullptr)
			return;

		auto effect = gameObject->GetComponent<Components::ParticleEffectComponent>();

		if (effect == nullptr)
			return;

		auto map = &subscribers_;

		if (effect->IsAnimated())
			map = &animatedSubscribers_;

		(*map)[gameObject->GetId()].particles = &effect->GetParticles();
		(*map)[gameObject->GetId()].blendFunction = effect->GetBlendType();

		if (effect->GetTexture() != nullptr)
		{
			(*map)[gameObject->GetId()].texture = effect->GetTexture();
		}
	}
	void ParticlesRenderer::UnSubscribe(CGameObject * gameObject)
	{
		if (gameObject == nullptr)
			return;
		if (subscribers_.count(gameObject->GetId()))
			subscribers_.erase(gameObject->GetId());
	}
	void ParticlesRenderer::UnSubscribeAll()
	{
		subscribers_.clear();
	}
	void ParticlesRenderer::ReloadShaders()
	{
		shader.Stop();
		shader.Reload();
		animatedShader_.Reload();
		Init();
	}
	void ParticlesRenderer::PrepareFrame()
	{
		target->BindToDraw();
		graphicsApi_->DisableBlend();
		graphicsApi_->EnableBlend();		
		graphicsApi_->DisableDepthMask();
		graphicsApi_->DisableCulling();
	}
	void ParticlesRenderer::ClearFrame()
	{
		graphicsApi_->EnableCulling();
		graphicsApi_->EnableDepthMask();
		graphicsApi_->SetBlendFunction(BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
		graphicsApi_->DisableBlend();
	}
	void ParticlesRenderer::RenderSubscribes(const mat4& viewMatrix)
	{
		for (const auto& sub : subscribers_)
		{
			auto& effect = sub.second;
			if (effect.particles == nullptr)
				continue;

			if (effect.texture != nullptr)
				graphicsApi_->ActiveTexture(0, effect.texture->GetId());

			RenderParticles(*effect.particles, viewMatrix);
		}
	}
	void ParticlesRenderer::RenderAnimatedSubscribes(const mat4 & viewMatrix)
	{
		for (const auto& sub : animatedSubscribers_)
		{
			auto& effect = sub.second;
			graphicsApi_->SetBlendFunction(effect.blendFunction);
			if (effect.particles == nullptr)
				continue;

			if (effect.texture != nullptr)
			{
				animatedShader_.Load(AnimatedParticlesShader::UniformLocation::NumberOfRows, static_cast<float>(effect.texture->numberOfRows));
				graphicsApi_->ActiveTexture(0, effect.texture->GetId());
			}

			for (const auto& particle : *effect.particles)
			{
				auto blendFactor = (particle.elapsedTime / particle.lifeTime);
				if (blendFactor > 1.f)
				{
					continue;
				}
				auto count = (effect.texture->numberOfRows * effect.texture->numberOfRows );
				auto textureIndex = static_cast<uint32>(std::floor(blendFactor * static_cast<float>(count)));
				auto offset1 = effect.texture->GetTextureOffset(textureIndex);
				auto nextIndex = textureIndex < count - 1 ? textureIndex + 1 : textureIndex;
				auto offset2 = effect.texture->GetTextureOffset(nextIndex);

				if (effect.texture != nullptr)
				{
					animatedShader_.Load(AnimatedParticlesShader::UniformLocation::TextureOffset, vec4(offset1.x, offset1.y, offset2.x, offset2.y));
				}

				animatedShader_.Load(AnimatedParticlesShader::UniformLocation::BlendFactor, wielomian(blendFactor) );
				auto modelViewMatrix = UpdateModelViewMatrix(particle.position, particle.rotation, particle.scale, viewMatrix);
				animatedShader_.Load(AnimatedParticlesShader::UniformLocation::ModelViewMatrix, modelViewMatrix);
				graphicsApi_->RenderQuad();
			}
		}
	}
	mat4 ParticlesRenderer::UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix)
	{
		mat4 modelMatrix(1.f);
		modelMatrix = glm::translate(position);

		modelMatrix[0][0] = viewMatrix[0][0];
		modelMatrix[0][1] = viewMatrix[1][0];
		modelMatrix[0][2] = viewMatrix[2][0];
		modelMatrix[1][0] = viewMatrix[0][1];
		modelMatrix[1][1] = viewMatrix[1][1];
		modelMatrix[1][2] = viewMatrix[2][1];
		modelMatrix[2][0] = viewMatrix[0][2];
		modelMatrix[2][1] = viewMatrix[1][2];
		modelMatrix[2][2] = viewMatrix[2][2];

		//modelMatrix *= glm::rotate(rotation, 0.f, 0.f, 1.f);
		scale *= 0.125f;
		modelMatrix *= glm::scale(scale, scale, scale);
		auto modelViewMatrix = viewMatrix * modelMatrix;
		return modelViewMatrix;
	}

	void ParticlesRenderer::RenderParticles(const std::vector<Particle>& particles, const mat4& viewMatrix)
	{
		for (const auto& particle : particles)
		{
			auto modelViewMatrix = UpdateModelViewMatrix(particle.position, particle.rotation, particle.scale, viewMatrix);
			shader.Load(ParticlesShader::UniformLocation::ModelViewMatrix, modelViewMatrix);
			graphicsApi_->RenderQuad();
		}
	}

} // GameEngine
