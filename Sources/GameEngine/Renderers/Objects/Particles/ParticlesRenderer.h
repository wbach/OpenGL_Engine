#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Objects/Particle.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "ParticleInputBuffer.h"

namespace GameEngine
{
struct Time;
class Texture;
class Projection;
struct RendererContext;

struct ParticleSubscriber
{
    bool isAnimated  = false;
    Texture* texture = nullptr;
    GraphicsApi::BlendFunctionType blendFunction;
    std::vector<Particle>* particles = nullptr;
};

class ParticlesRenderer : public IRenderer
{
    typedef std::unordered_map<uint32, ParticleSubscriber> SubscribesMap;

public:
    ParticlesRenderer(RendererContext& context);

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    bool IsInit() const;
    void InitShaderBuffer();
    void StartShader();
    void StopShader();
    void PrepareFrame();
    void ClearFrame();
    void RenderSubscribes(const mat4& viewMatrix);
    void RenderParticles(const ParticleSubscriber& effect, const mat4& viewMatrix);
    void RenderInstances(size_t);
    mat4 UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix);
    void UpdateTexture(Texture*);
    void ReallocationParticlesData(size_t);
    void GetParticleData(const std::vector<Particle>& particles, const mat4& viewMatrix);
    vec4 GetTextureOffsets(float blendFactor);

private:
    RendererContext& context_;
    ShaderProgram shader_;
    ShaderProgram animatedShader_;
    SubscribesMap subscribers_;
    GraphicsApi::ID particleObjecId;
    GraphicsApi::ID aniamtedParticleObjecId;
    GraphicsApi::ID staticParticleObjecId;
    GraphicsApi::ID particleInputBufferId;
    ParticleInputBuffer particleInputBuffer;

private:
    bool currentUseAnimation;
    uint32 textureNumberOfrows;
    std::vector<vec4> offsets_;
    std::vector<float> blendFactors_;
    std::vector<mat4> transformsParticles_;
};
}  // namespace GameEngine
