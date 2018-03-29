#include "ParticlesRenderer.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"
#include "../Framebuffer/FrameBuffer.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	ParticlesRenderer::ParticlesRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, shader(graphicsApi)
		, graphicsApi_(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void ParticlesRenderer::Init()
	{
		shader.Init();
		shader.Start();
		shader.Load(ParticlesShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());
		shader.Stop();
	}
	void ParticlesRenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void ParticlesRenderer::Render(GameEngine::Scene * scene)
	{
		target->BindToDraw();
		shader.Start();
		graphicsApi_->EnableBlend();
		graphicsApi_->SetBlendFunction(BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
		graphicsApi_->DisableDepthMask();
		graphicsApi_->DisableCulling();

		for (const auto& sub : subscribers_)
		{
			auto& effect = sub.second;
			if (effect.particles == nullptr)
				continue;

			if (effect.texture != nullptr)
				graphicsApi_->ActiveTexture(0, effect.texture->GetId());

			for (const auto& particle : *effect.particles)
			{
				UpdateModelViewMatrix(particle.position, particle.rotation, particle.scale, scene->GetCamera()->GetViewMatrix());
				graphicsApi_->RenderQuad();
			}
		}

		graphicsApi_->EnableCulling();
		graphicsApi_->EnableDepthMask();
		graphicsApi_->DisableBlend();
		shader.Stop();
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

		subscribers_[gameObject->GetId()].particles = &effect->GetParticles();

		if (effect->GetTexture() != nullptr)
		{
			subscribers_[gameObject->GetId()].texture = effect->GetTexture();
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
		Init();
	}
	void ParticlesRenderer::UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix)
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
		shader.Load(ParticlesShader::UniformLocation::ModelViewMatrix, modelViewMatrix);
	}

} // GameEngine
