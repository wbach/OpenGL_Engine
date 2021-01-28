#include "ParticleEffectComponent.h"

#include <algorithm>

#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR         = "ParticleEffectComponent";
const std::string CSTR_VELOCITY         = "velocity";
const std::string CSTR_GRAVITY_EFFECT   = "gravityEffect";
const std::string CSTR_LIFE_TIME        = "lifeTime";
const std::string CSTR_PARTICLE         = "particle";
const std::string CSTR_PARTICLE_PER_SER = "particlePerSec";
const std::string CSTR_PARTICLE_LIMT    = "particleLimt";
const std::string CSTR_EMIT_FUNCTION    = "emitFunciton";
const std::string CSTR_BLEND_TYPE       = "blendType";
const std::string CSTR_IS_ANIMATED      = "isAnimated";
const std::string CSTR_SPEED            = "speed";

}  // namespace

ParticleEffectComponent::ParticleEffectComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(ParticleEffectComponent).hash_code(), componentContext, gameObject)
    , texture_(nullptr)
    , particlesSpeed_(10.f)
    , particlesPerSecond_(10)
    , particlesLimit_(10000)
    , isAnimated_(false)
    , blendFunction_(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA)
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

Particle ReadParticle(const TreeNode& node)
{
    Particle particle;

    ::Read(*node.getChild(CSTR_POSITION), particle.position);
    ::Read(node.getChild(CSTR_ROTATION)->value_, particle.rotation);
    ::Read(node.getChild(CSTR_SCALE)->value_, particle.scale);
    ::Read(node.getChild(CSTR_VELOCITY), particle.velocity);
    ::Read(node.getChild(CSTR_GRAVITY_EFFECT), particle.gravityEffect);
    ::Read(node.getChild(CSTR_LIFE_TIME)->value_, particle.lifeTime);

    return particle;
}

void ParticleEffectComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<ParticleEffectComponent>(componentContext, gameObject);

        auto particle = ReadParticle(*node.getChild(CSTR_PARTICLE));
        component->SetParticle(particle);
        component->SetTexture(node.getChild(CSTR_TEXTURE)->value_);
        component->SetParticlesPerSec(std::stoul(node.getChild(CSTR_PARTICLE_PER_SER)->value_));
        component->SetBlendFunction(
            static_cast<GraphicsApi::BlendFunctionType>(std::stoi(node.getChild(CSTR_BLEND_TYPE)->value_)));

        auto emitFunctionName = node.getChild(CSTR_EMIT_FUNCTION)->value_;
        auto emitFunction     = componentContext.scene_.GetParticleEmitFunction(emitFunctionName);
        if (emitFunction)
        {
            component->SetEmitFunction(emitFunctionName, *emitFunction);
        }
        component->SetSpeed(std::stof(node.getChild(CSTR_SPEED)->value_));
        auto animated = Utils::StringToBool(node.getChild(CSTR_IS_ANIMATED)->value_);

        if (animated)
        {
            component->EnableAnimation();
        }
        component->SetParticlesLimit(std::stoul(node.getChild(CSTR_PARTICLE_LIMT)->value_));

        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void ParticleEffectComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
