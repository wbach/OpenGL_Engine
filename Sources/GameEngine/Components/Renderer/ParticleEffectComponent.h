#pragma once
#include "GameEngine/Components/AbstractComponent.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Objects/RenderAble/Paricle/Particle.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
	namespace Components
	{
		typedef std::function<Particle( const Particle&)> EmitFunction;

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
			void SetSpeed(float s) { particlesSpeed_ = s; }
			bool IsAnimated() { return isAnimated_; }
			void UseAnimation() { isAnimated_ = true; }
			void SetEmitFunction(EmitFunction f) { emitFunction_ = f; }
			void SetBlendFunction(BlendFunctionType type) { blendFunction_ = type; }
			BlendFunctionType GetBlendType() { return blendFunction_; }

		private:
			void Subscribe();
			void UnSubscribe();
			void Update();
			void Start();
			void EmitParticle(const vec3&);
			Particle DefaultEmitFunction(const Particle&);

		private:
			CTexture* texture_;
			Particle referenceParticle_;
			float particlesSpeed_;
			uint32 particlesPerSecond_;
			uint32 particlesCount_;
			std::vector<Particle> particles_;
			bool isAnimated_;
			EmitFunction emitFunction_;
			BlendFunctionType blendFunction_;

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
			texture_ = resourceManager_->GetTextureLaoder().LoadTexture(filename, true, true, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);
		}
		void ParticleEffectComponent::AddParticle(const Particle& particle)
		{
			particles_.push_back(particle);
		}
	} // Components
} // GameEngine
