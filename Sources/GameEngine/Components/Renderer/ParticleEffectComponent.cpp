#include "ParticleEffectComponent.h"
#include "InsertionSort.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include <algorithm>

namespace GameEngine
{
	namespace Components
	{
		ComponentsType ParticleEffectComponent::type = ComponentsType::ParticleEffect;

		ParticleEffectComponent::ParticleEffectComponent()
			: AbstractComponent(ComponentsType::ParticleEffect)
			, texture_(nullptr)
			, particlesSpeed_(10.f)
			, particlesPerSecond_(10)
			, particlesLimit_(10000)
			, isAnimated_(false)
			, blendFunction_(BlendFunctionType::ALPHA_ONE_MINUS_ALPHA)
			, rest(0)
		{
			emitFunction_ = std::bind(&ParticleEffectComponent::DefaultEmitFunction, this, std::placeholders::_1);
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
			if (emitFunction_ == nullptr)
				return;

			Particle particle = emitFunction_(referenceParticle_);
			particle.velocity = glm::normalize(particle.velocity) * particlesSpeed_;
			AddParticle(particle);
		}
		Particle ParticleEffectComponent::DefaultEmitFunction(const Particle & referenceParticle)
		{
			Particle particle = referenceParticle;
			float dirX = static_cast<float>(rand() % 100) / 100.f * 2.f - 1.f;
			float dirZ = static_cast<float>(rand() % 100) / 100.f * 2.f - 1.f;
			particle.velocity = vec3(dirX, 1, dirZ);
			return particle;
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

			auto countF = static_cast<float>(particlesPerSecond_);
			auto toEmitF = countF * time_->deltaTime;
			auto toEmit = static_cast<uint32>(toEmitF);

			if (toEmit > std::numeric_limits<float>::min())
			{
				rest += fmod(toEmitF, toEmit);
				if (rest > 1.f)
				{
					++toEmit;
					rest -= 1.f;
				}
			}

			for (uint32 x = 0; x < toEmit; ++x)
			{
				if (particlesLimit_ > particles_.size())
					EmitParticle(vec3());
			}

			auto& camPosition = GetCamera().GetPosition();

			std::sort(particles_.begin(), particles_.end(), [&camPosition](const Particle& l, const Particle& r)
			{
				auto distance = glm::distance(l.position, camPosition);
				auto distance2 = glm::distance(r.position, camPosition);
				return distance > distance2;
			});
		}
	} // Components
} // GameEngine
