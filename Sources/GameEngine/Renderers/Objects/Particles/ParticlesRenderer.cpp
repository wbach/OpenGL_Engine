#include "ParticlesRenderer.h"

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GraphicsApi/ShaderProgramType.h"

namespace GameEngine
{
float quadraticFunction(float blend)
{
    return -1.f * blend * blend + 2 * blend;
}
float polynomialFunction(float blend)
{
    auto r = -3.2f * blend * blend * blend * blend + 8.8f * blend * blend * blend - 9.2f * blend * blend +
             4.6f * blend - 4e-12f;
    if (r < 0.f)
        return 0.f;
    if (r > 1.f)
        return 1.f;
    return r;
}
ParticlesRenderer::ParticlesRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Particles)
    , animatedShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::AnimatedParticles)
    , particleObjecId(0)
    , currentUseAnimation(false)
    , textureNumberOfrows(1)
{
}
void ParticlesRenderer::init()
{
    aniamtedParticleObjecId = context_.graphicsApi_.CreateAnimatedParticle();
    staticParticleObjecId   = context_.graphicsApi_.CreateParticle();

    shader_.Init();
    animatedShader_.Init();

    InitShaderBuffer();
}
void ParticlesRenderer::render()
{
    if (IsInit() and not subscribers_.empty() and context_.scene_)
    {
        PrepareFrame();
        RenderSubscribes(context_.scene_->GetCamera().GetViewMatrix());
        ClearFrame();
    }
}
void ParticlesRenderer::subscribe(GameObject& gameObject)
{
    if (subscribers_.find(gameObject.GetId()) != subscribers_.end())
        return;

    auto effect = gameObject.GetComponent<Components::ParticleEffectComponent>();

    if (effect)
    {
        subscribers_.insert(
            {gameObject.GetId(),
             {effect->IsAnimated(), effect->GetTexture(), effect->GetBlendType(), &effect->GetParticles()}});
    }
}
void ParticlesRenderer::unSubscribe(GameObject& gameObject)
{
    if (subscribers_.count(gameObject.GetId()))
        subscribers_.erase(gameObject.GetId());
}
void ParticlesRenderer::unSubscribeAll()
{
    subscribers_.clear();
}
void ParticlesRenderer::reloadShaders()
{
    shader_.Reload();
    animatedShader_.Reload();
}
void ParticlesRenderer::PrepareFrame()
{
    context_.graphicsApi_.DisableBlend();
    context_.graphicsApi_.EnableBlend();
    context_.graphicsApi_.DisableDepthMask();
    context_.graphicsApi_.DisableCulling();
}
void ParticlesRenderer::ClearFrame()
{
    context_.graphicsApi_.EnableCulling();
    context_.graphicsApi_.EnableDepthMask();
    context_.graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA);
    context_.graphicsApi_.DisableBlend();
}
void ParticlesRenderer::RenderSubscribes(const mat4& viewMatrix)
{
    for (const auto& sub : subscribers_)
    {
        auto& effect = sub.second;
        if (not effect.particles)
            continue;

        currentUseAnimation = effect.isAnimated;
        particleObjecId     = currentUseAnimation ? aniamtedParticleObjecId : staticParticleObjecId;
        StartShader();
        UpdateTexture(effect.texture);
        RenderParticles(effect, viewMatrix);
        StopShader();
    }
}

void ParticlesRenderer::RenderParticles(const ParticleSubscriber& effect, const mat4& viewMatrix)
{
    // context_.graphicsApi_.SetBlendFunction(effect.blendFunction);
    UpdateTexture(effect.texture);

    auto particlesSize = effect.particles->size();
    ReallocationParticlesData(particlesSize);
    GetParticleData(*effect.particles, viewMatrix);
    RenderInstances(particlesSize);
}

void ParticlesRenderer::RenderInstances(size_t size)
{
    context_.graphicsApi_.UpdateMatrixes(*particleObjecId, transformsParticles_);

    if (currentUseAnimation)
    {
        context_.graphicsApi_.UpdateOffset(*particleObjecId, offsets_);
        context_.graphicsApi_.UpdateBlend(*particleObjecId, blendFactors_);
    }
    context_.graphicsApi_.RenderMeshInstanced(*particleObjecId, static_cast<uint32>(size));
}
bool ParticlesRenderer::IsInit() const
{
    return shader_.IsReady() and animatedShader_.IsReady() and particleObjecId.has_value() and
           aniamtedParticleObjecId.has_value() and staticParticleObjecId.has_value() and
           particleInputBufferId.has_value();
}
void ParticlesRenderer::InitShaderBuffer()
{
    particleInputBuffer.projectionMatrix          = context_.projection_.GetProjectionMatrix();
    particleInputBuffer.textureNumberOfRows.value = 0;

    particleInputBufferId =
        context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(ParticleInputBuffer));
    context_.graphicsApi_.UpdateShaderBuffer(*particleInputBufferId, &particleInputBuffer);
}
void ParticlesRenderer::StartShader()
{
    currentUseAnimation ? animatedShader_.Start() : shader_.Start();
}
void ParticlesRenderer::StopShader()
{
    currentUseAnimation ? animatedShader_.Stop() : shader_.Stop();
}
mat4 ParticlesRenderer::UpdateModelViewMatrix(const vec3& position, float, float scale, const mat4& viewMatrix)
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

    // modelMatrix *= glm::rotate(rotation, 0.f, 0.f, 1.f);
    scale *= 0.125f;
    modelMatrix *= glm::scale(vec3(scale));
    auto modelViewMatrix = viewMatrix * modelMatrix;
    return modelViewMatrix;
}

void ParticlesRenderer::UpdateTexture(Texture* texture)
{
    if (not texture)
    {
        return;
    }

    if (currentUseAnimation)
    {
        textureNumberOfrows = texture->getNumberOfRows();

        particleInputBuffer.textureNumberOfRows = static_cast<float>(textureNumberOfrows);
        context_.graphicsApi_.UpdateShaderBuffer(*particleInputBufferId, &particleInputBuffer);
        context_.graphicsApi_.BindShaderBuffer(*particleInputBufferId);
    }

    if (texture->GetGraphicsObjectId())
        context_.graphicsApi_.ActiveTexture(0, *texture->GetGraphicsObjectId());
}

void ParticlesRenderer::GetParticleData(const std::vector<Particle>& particles, const mat4& viewMatrix)
{
    for (const auto& particle : particles)
    {
        auto blendFactor = (particle.elapsedTime / particle.lifeTime);
        if (blendFactor > 1.f)
        {
            continue;
        }

        auto modelViewMatrix = UpdateModelViewMatrix(particle.position, particle.rotation, particle.scale, viewMatrix);
        transformsParticles_.push_back(modelViewMatrix);

        if (currentUseAnimation)
        {
            offsets_.push_back(GetTextureOffsets(blendFactor));
            blendFactors_.push_back(blendFactor);
        }
    }
}

void ParticlesRenderer::ReallocationParticlesData(size_t size)
{
    transformsParticles_.clear();
    transformsParticles_.reserve(size);

    if (currentUseAnimation)
    {
        offsets_.clear();
        blendFactors_.clear();

        offsets_.reserve(size);
        blendFactors_.reserve(size);
    }
}

vec4 ParticlesRenderer::GetTextureOffsets(float blendFactor)
{
    auto count        = (textureNumberOfrows * textureNumberOfrows);
    auto textureIndex = static_cast<uint32>(std::floor(blendFactor * static_cast<float>(count)));
    auto offset1      = GameEngine::GetTextureOffset(textureIndex, textureNumberOfrows);
    auto nextIndex    = textureIndex < count - 1 ? textureIndex + 1 : textureIndex;
    auto offset2      = GameEngine::GetTextureOffset(nextIndex, textureNumberOfrows);

    return vec4(offset1.x, offset1.y, offset2.x, offset2.y);
}

}  // namespace GameEngine
