#pragma once
#include "Particle.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace Components
{
typedef std::function<Particle(const Particle&)> EmitFunction;

class ParticleEffectComponent : public BaseComponent
{
public:
    ParticleEffectComponent(const ComponentContext& componentContext, GameObject& gameObject);
    virtual void ReqisterFunctions() override;
    inline std::vector<Particle>& GetParticles();
    inline Texture* GetTexture();
    inline void AddParticle(const Particle& particle);
    ParticleEffectComponent& SetTexture(const std::string& filename);
    ParticleEffectComponent& SetParticle(const Particle& particle);
    ParticleEffectComponent& SetParticlesPerSec(uint32 c);
    ParticleEffectComponent& SetParticlesLimit(uint32 limit);
    ParticleEffectComponent& SetSpeed(float s);
    ParticleEffectComponent& SetEmitFunction(EmitFunction f);
    ParticleEffectComponent& SetBlendFunction(BlendFunctionType type);
    bool IsAnimated()
    {
        return isAnimated_;
    }
    ParticleEffectComponent& EnableAnimation()
    {
        isAnimated_ = true;
        return *this;
    }
    BlendFunctionType GetBlendType()
    {
        return blendFunction_;
    }
    uint32 GetParticlesCount() const
    {
        return particles_.size();
    }

private:
    void Subscribe();
    void UnSubscribe();
    void Update();
    void UpdateParticlesState();
    void Start();
    void EmitParticle(const vec3&);
    void EmitParticles();
    void SortParticlesByCameraDistance();
    uint32 CalculateToEmitParticles();
    Particle DefaultEmitFunction(const Particle&);

private:
    Texture* texture_;
    Particle referenceParticle_;
    float particlesSpeed_;
    uint32 particlesPerSecond_;
    uint32 particlesLimit_;
    std::vector<Particle> particles_;
    bool isAnimated_;
    EmitFunction emitFunction_;
    BlendFunctionType blendFunction_;
    float rest;

public:
    static ComponentsType type;
};

std::vector<Particle>& ParticleEffectComponent::GetParticles()
{
    return particles_;
}
Texture* ParticleEffectComponent::GetTexture()
{
    return texture_;
}
void ParticleEffectComponent::AddParticle(const Particle& particle)
{
    particles_.push_back(particle);
}
}  // Components
}  // GameEngine
