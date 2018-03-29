#include "ParticleEffectComponent.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType ParticleEffectComponent::type = ComponentsType::ParticleEffect;

		ParticleEffectComponent::ParticleEffectComponent()
			: AbstractComponent(ComponentsType::ParticleEffect)
		{
			particles_.reserve(10000);
		}
		void ParticleEffectComponent::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&ParticleEffectComponent::Subscribe, this));
			RegisterFunction(FunctionType::Update, std::bind(&ParticleEffectComponent::Update, this));
		}
		void ParticleEffectComponent::Subscribe()
		{
			if (renderersManager_ == nullptr)
				return;

			renderersManager_->Subscribe(thisObject);
		}
		void ParticleEffectComponent::Start()
		{
			
		}
		void ParticleEffectComponent::EmitParticle(const vec3&)
		{
			Particle particle = referenceParticle_;

			float dirX = static_cast<float>(rand() % 100) / 100.f * 2.f - 1.f;
			float dirZ = static_cast<float>(rand() % 100) / 100.f * 2.f - 1.f;
			vec3 velocity(dirX, 1, dirZ);
			particle.velocity = glm::normalize(velocity) * particlesSpeed_;
			AddParticle(particle);
		}
		void ParticleEffectComponent::SetParticle(const Particle& particle)
		{
			referenceParticle_ = particle;
		}
		void ParticleEffectComponent::UnSubscribe()
		{
			if (renderersManager_ == nullptr)
				return;
		}
		void ParticleEffectComponent::Update()
		{
			for (auto iter = particles_.begin(); iter != particles_.end();)
			{
				auto isAlive = iter->update(time_->deltaTime, -9.81f);
				if (!isAlive)
					iter = particles_.erase(iter);
				else
					++iter;
			}

			
			auto particlesToCreate = static_cast<float>(particlesPerSecond_) * time_->deltaTime; 
			int count  = static_cast<uint32>(particlesToCreate);
			auto partialParticle = fmod(particlesToCreate, 1);

			if (particlesCount_ - particles_.size()  > 0)
				EmitParticle(vec3());

			/*float r = static_cast<float>(rand() % 1000) / 1000.f;
			if ( r < partialParticle)
				EmitParticle(vec3());*/
		}
	} // Components
} // GameEngine
