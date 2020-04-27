#include "ParticleEffectComponent.h"

#include <algorithm>

#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType ParticleEffectComponent::type = ComponentsType::ParticleEffect;

ParticleEffectComponent::ParticleEffectComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::ParticleEffect, componentContext, gameObject)
    , texture_(nullptr)
    , particlesSpeed_(10.f)
    , particlesPerSecond_(10)
    , particlesLimit_(10000)
    , isAnimated_(false)
    , blendFunction_(GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA)
    , rest(0)
    , isSubscribed_(false)
{
    emitFunction_ = std::bind(&ParticleEffectComponent::DefaultEmitFunction, this, std::placeholders::_1);
    particles_.reserve(10000);
}
ParticleEffectComponent::~ParticleEffectComponent()
{
}

void ParticleEffectComponent::CleanUp()
{
    UnSubscribe();
    DeleteTexture();
}
void ParticleEffectComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&ParticleEffectComponent::Subscribe, this));
    RegisterFunction(FunctionType::Update, std::bind(&ParticleEffectComponent::Update, this));
}
void ParticleEffectComponent::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void ParticleEffectComponent::EmitParticle(const vec3&)
{
    if (emitFunction_ == nullptr)
        return;

    Particle particle = emitFunction_(referenceParticle_);
    particle.velocity = glm::normalize(particle.velocity) * particlesSpeed_;
    AddParticle(particle);
}
Particle ParticleEffectComponent::DefaultEmitFunction(const Particle& referenceParticle)
{
    Particle particle = referenceParticle;
    float dirX        = static_cast<float>(rand() % 100) / 100.f * 2.f - 1.f;
    float dirZ        = static_cast<float>(rand() % 100) / 100.f * 2.f - 1.f;
    particle.velocity = vec3(dirX, 1, dirZ);
    return particle;
}
void ParticleEffectComponent::DeleteTexture()
{
    if (texture_)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*texture_);
    }
    texture_ = nullptr;
}
ParticleEffectComponent& ParticleEffectComponent::SetParticle(const Particle& particle)
{
    referenceParticle_ = particle;
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetParticlesPerSec(uint32 c)
{
    particlesPerSecond_ = c;
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetParticlesLimit(uint32 limit)
{
    particlesLimit_ = limit;
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetSpeed(float s)
{
    particlesSpeed_ = s;
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetEmitFunction(const std::string& emitFunctionName, EmitFunction f)
{
    emitFunctionName_ = emitFunctionName;
    SetEmitFunction(f);
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetEmitFunction(EmitFunction f)
{
    emitFunction_ = f;
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetBlendFunction(GraphicsApi::BlendFunctionType type)
{
    blendFunction_ = type;
    return *this;
}
ParticleEffectComponent& ParticleEffectComponent::SetTexture(const std::string& filename)
{
    textureFile_ = filename;
    TextureParameters params;
    params.flipMode = TextureFlip::VERTICAL;

    DeleteTexture();
    texture_ = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(filename, params);

    return *this;
}
void ParticleEffectComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void ParticleEffectComponent::Update()
{
    UpdateParticlesState();
    EmitParticles();
    SortParticlesByCameraDistance();
}
void ParticleEffectComponent::UpdateParticlesState()
{
    for (auto iter = particles_.begin(); iter != particles_.end();)
    {
        auto isAlive = iter->update(componentContext_.time_.deltaTime, -9.81f);
        if (!isAlive)
            iter = particles_.erase(iter);
        else
            ++iter;
    }
}
uint32 ParticleEffectComponent::CalculateToEmitParticles()
{
    auto countF  = static_cast<float>(particlesPerSecond_);
    auto toEmitF = countF * componentContext_.time_.deltaTime;
    auto toEmit  = static_cast<uint32>(toEmitF);

    if (toEmitF > std::numeric_limits<float>::min())
    {
        rest += static_cast<float>(fmodf(toEmitF, 1.f));
        if (rest > 1.f)
        {
            ++toEmit;
            rest -= 1.f;
        }
    }
    return toEmit;
}
void ParticleEffectComponent::EmitParticles()
{
    auto toEmit = CalculateToEmitParticles();

    for (uint32 x = 0; x < toEmit; ++x)
    {
        if (particlesLimit_ > particles_.size())
            EmitParticle(vec3());
    }
}
void ParticleEffectComponent::SortParticlesByCameraDistance()
{
    auto camPosition = componentContext_.camera_.GetPosition();

    std::sort(particles_.begin(), particles_.end(), [&camPosition](const Particle& l, const Particle& r) {
        auto distance  = glm::distance(l.position, camPosition);
        auto distance2 = glm::distance(r.position, camPosition);
        return distance > distance2;
    });
}
}  // namespace Components
}  // namespace GameEngine
