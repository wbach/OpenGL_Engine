#include "ParticlesRenderer.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Shaders/ParticlesShadersUniforms.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GraphicsApi/ShadersTypes.h"

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
    , particleObjecId(0)
    , currentUseAnimation(false)
    , textureNumberOfrows(1)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Particles);
    animatedShader_ = context.shaderFactory_.create(GraphicsApi::Shaders::AnimatedParticles);
    __RegisterRenderFunction__(RendererFunctionType::POSTUPDATE, ParticlesRenderer::Render);
}
void ParticlesRenderer::Init()
{
    InitShaders();
    aniamtedParticleObjecId = context_.graphicsApi_.CreateAnimatedParticle();
    staticParticleObjecId   = context_.graphicsApi_.CreateParticle();
}
void ParticlesRenderer::Render(const Scene& scene, const Time&)
{
    if (subscribers_.empty())
        return;

    PrepareFrame();
    RenderSubscribes(scene.GetCamera().GetViewMatrix());
    ClearFrame();
}
void ParticlesRenderer::Subscribe(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

    auto effect = gameObject->GetComponent<Components::ParticleEffectComponent>();

    if (effect == nullptr)
        return;

    subscribers_[gameObject->GetId()].particles     = &effect->GetParticles();
    subscribers_[gameObject->GetId()].blendFunction = effect->GetBlendType();
    subscribers_[gameObject->GetId()].isAnimated    = effect->IsAnimated();

    if (effect->GetTexture() != nullptr)
    {
        subscribers_[gameObject->GetId()].texture = effect->GetTexture();
    }
}
void ParticlesRenderer::UnSubscribe(GameObject* gameObject)
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
    shader_->Stop();
    shader_->Reload();
    animatedShader_->Reload();
    InitShaders();
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
    context_.graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
    context_.graphicsApi_.DisableBlend();
}
void ParticlesRenderer::RenderSubscribes(const mat4& viewMatrix)
{
    for (const auto& sub : subscribers_)
    {
        auto& effect = sub.second;
        if (effect.particles == nullptr)
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

void ParticlesRenderer::RenderInstances(uint32 size)
{
    context_.graphicsApi_.UpdateMatrixes(particleObjecId, transformsParticles_);

    if (currentUseAnimation)
    {
        context_.graphicsApi_.UpdateOffset(particleObjecId, offsets_);
        context_.graphicsApi_.UpdateBlend(particleObjecId, blendFactors_);
    }
    context_.graphicsApi_.RenderMeshInstanced(particleObjecId, size);
}
void ParticlesRenderer::StartShader()
{
    currentUseAnimation ? animatedShader_->Start() : shader_->Start();
}
void ParticlesRenderer::StopShader()
{
   // currentUseAnimation ? animatedShader_->Stop() : shader_->Stop();
}
mat4 ParticlesRenderer::UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix)
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
    modelMatrix *= glm::scale(scale, scale, scale);
    auto modelViewMatrix = viewMatrix * modelMatrix;
    return modelViewMatrix;
}

void ParticlesRenderer::InitShaders()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(ParticlesShadersUniforms::ProjectionMatrix, context_.projection_.GetProjectionMatrix());
    shader_->Stop();

    animatedShader_->Init();
    animatedShader_->Start();
    animatedShader_->Load(ParticlesShadersUniforms::ProjectionMatrix,
                         context_.projection_.GetProjectionMatrix());
    animatedShader_->Stop();
}

void ParticlesRenderer::UpdateTexture(Texture* texture)
{
    if (texture == nullptr)
    {
        return;
    }

    if (currentUseAnimation)
    {
        textureNumberOfrows = texture->numberOfRows;
        animatedShader_->Load(ParticlesShadersUniforms::NumberOfRows,
                             static_cast<float>(textureNumberOfrows));
    }

    context_.graphicsApi_.ActiveTexture(0, texture->GetId());
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

void ParticlesRenderer::ReallocationParticlesData(uint32 size)
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

}  // GameEngine
