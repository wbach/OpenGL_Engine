#pragma once
#include "GameEngine/Components/AbstractComponent.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Objects/RenderAble/Paricle/Particle.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
	namespace Components
	{
		class ParticleEffectComponent : public AbstractComponent
		{
		public:
			ParticleEffectComponent();
			virtual void ReqisterFunctions() override;
			inline std::vector<Particle>& GetParticles();
			inline CTexture* GetTexture();
			inline void SetTexture(const std::string& filename);
			inline void AddParticle(const Particle& particle);
			void SetParticle(const Particle& particle);
			void SetParticlesCount(uint32 c) { particlesCount_ = c; }

		private:
			void Subscribe();
			void UnSubscribe();
			void Update();
			void Start();
			void EmitParticle(const vec3&);

		private:
			CTexture* texture_;
			Particle referenceParticle_;
			float particlesSpeed_ = 10.f;
			uint32 particlesPerSecond_ = 10;
			uint32 particlesCount_ = 100;
			std::vector<Particle> particles_;

		public:
			static ComponentsType type;
		};

		std::vector<Particle>& ParticleEffectComponent::GetParticles()
		{
			return particles_;
		}
		CTexture* ParticleEffectComponent::GetTexture()
		{
			return texture_;
		}
		void ParticleEffectComponent::SetTexture(const std::string& filename)
		{
			texture_ = resourceManager_->GetTextureLaoder().LoadTexture(filename, true, true, ObjectTextureType::MATERIAL);
		}
		void ParticleEffectComponent::AddParticle(const Particle& particle)
		{
			particles_.push_back(particle);
		}
	} // Components
} // GameEngine
