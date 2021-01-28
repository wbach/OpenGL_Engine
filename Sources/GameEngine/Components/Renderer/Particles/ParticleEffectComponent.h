#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Objects/Particle.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace Components
{
class ParticleEffectComponent : public BaseComponent
{
public:
    ParticleEffectComponent(ComponentContext& componentContext, GameObject& gameObject);
    ~ParticleEffectComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;

    ParticleEffectComponent& SetTexture(const std::string& filename);
    ParticleEffectComponent& SetParticle(const Particle& particle);
    ParticleEffectComponent& SetParticlesPerSec(uint32 c);
    ParticleEffectComponent& SetParticlesLimit(uint32 limit);
    ParticleEffectComponent& SetSpeed(float s);
    ParticleEffectComponent& SetEmitFunction(const std::string& emitFunctionName, EmitFunction f);
    ParticleEffectComponent& SetEmitFunction(EmitFunction f);
    ParticleEffectComponent& SetBlendFunction(GraphicsApi::BlendFunctionType type);

    inline Texture* GetTexture();
    inline ParticleEffectComponent& EnableAnimation();
    inline bool IsAnimated() const;
    inline float GetParticleSpeed() const;
    inline uint32 GetParticlesCount() const;
    inline uint32 GetParticleLimit() const;
    inline uint32 GetParticlePerSec() const;
    inline GraphicsApi::BlendFunctionType GetBlendType() const;
    inline std::vector<Particle>& GetParticles();
    inline const Particle& GetReferenceParticle() const;
    inline const std::string& GetTextureFile() const;
    inline const std::string& GetEmitFunction() const;

private:
    inline void AddParticle(const Particle& particle);
    void Subscribe();
    void UnSubscribe();
    void Update();
    void UpdateParticlesState();
    void EmitParticle(const vec3&);
    void EmitParticles();
    void SortParticlesByCameraDistance();
    uint32 CalculateToEmitParticles();
    Particle DefaultEmitFunction(const Particle&);
    void DeleteTexture();

private:
    Texture* texture_;
    Particle referenceParticle_;
    float particlesSpeed_;
    uint32 particlesPerSecond_;
    uint32 particlesLimit_;
    std::vector<Particle> particles_;
    bool isAnimated_;
    EmitFunction emitFunction_;
    GraphicsApi::BlendFunctionType blendFunction_;
    float rest;
    std::string textureFile_;
    std::string emitFunctionName_;
    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
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
bool ParticleEffectComponent::IsAnimated() const
{
    return isAnimated_;
}
ParticleEffectComponent& ParticleEffectComponent::EnableAnimation()
{
    isAnimated_ = true;
    return *this;
}
float ParticleEffectComponent::GetParticleSpeed() const
{
    return particlesSpeed_;
}
GraphicsApi::BlendFunctionType ParticleEffectComponent::GetBlendType() const
{
    return blendFunction_;
}
uint32 ParticleEffectComponent::GetParticlesCount() const
{
    return static_cast<uint32>(particles_.size());
}
const Particle& ParticleEffectComponent::GetReferenceParticle() const
{
    return referenceParticle_;
}
const std::string& ParticleEffectComponent::GetTextureFile() const
{
    return textureFile_;
}
uint32 ParticleEffectComponent::GetParticlePerSec() const
{
    return particlesPerSecond_;
}
const std::string& ParticleEffectComponent::GetEmitFunction() const
{
    return emitFunctionName_;
}
uint32 ParticleEffectComponent::GetParticleLimit() const
{
    return particlesLimit_;
}
}  // namespace Components
}  // namespace GameEngine
