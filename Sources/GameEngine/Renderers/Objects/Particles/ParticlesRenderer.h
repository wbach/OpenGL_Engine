#pragma once
#include "GameEngine/Objects/Particle.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
struct Time;
class Texture;
class Projection;
struct RendererContext;
class IShaderProgram;

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
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time&);

private:
    void InitShaders();
    void StartShader();
    void StopShader();
    void PrepareFrame();
    void ClearFrame();
    void RenderSubscribes(const mat4& viewMatrix);
    void RenderParticles(const ParticleSubscriber& effect, const mat4& viewMatrix);
    void RenderInstances(uint32 size);
    mat4 UpdateModelViewMatrix(const vec3& position, float rotation, float scale, const mat4& viewMatrix);
    void UpdateTexture(Texture* texture);
    void ReallocationParticlesData(uint32 size);
    void GetParticleData(const std::vector<Particle>& particles, const mat4& viewMatrix);
    vec4 GetTextureOffsets(float blendFactor);

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    std::unique_ptr<IShaderProgram> animatedShader_;
    SubscribesMap subscribers_;
    uint32 particleObjecId;
    uint32 aniamtedParticleObjecId;
    uint32 staticParticleObjecId;

private:
    bool currentUseAnimation;
    uint32 textureNumberOfrows;
    std::vector<vec4> offsets_;
    std::vector<float> blendFactors_;
    std::vector<mat4> transformsParticles_;
};
}  // namespace GameEngine
