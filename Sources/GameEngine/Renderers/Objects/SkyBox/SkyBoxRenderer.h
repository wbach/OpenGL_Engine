#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
struct Time;
class Model;
class Mesh;
class Texture;
struct Material;
class Projection;
struct RendererContext;
class IShaderProgram;

struct SkyBoxSubscriber
{
    Model* model_;
    Texture* dayTexture_;
    Texture* nightTexture_;
};

typedef std::unordered_map<uint32_t, SkyBoxSubscriber> SkyBoxSubscriberMap;

class SkyBoxRenderer : public IRenderer
{
public:
    SkyBoxRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time&);
    void BindTextures(const SkyBoxSubscriber& sub) const;
    void RenderSkyBoxMesh(const Mesh& mesh) const;
    void RenderSkyBoxModel(const SkyBoxSubscriber& sub);
    void PrepareShaderBeforeFrameRender(const Scene& scene);
    void BindCubeMapTexture(const Texture& texture, int id) const;
    void PrepareToRendering(const Scene& scene);
    void InitShader();
    void UpdateBuffer(const Scene& scene, const Time&);

private:
    struct PerMeshObject
    {
        AlignWrapper<vec4> fogColor_;
        AlignWrapper<float> blendFactor_;
    };

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    SkyBoxSubscriberMap subscribes_;
    GraphicsApi::ID perMeshObjectId_;
    GraphicsApi::ID perObjectUpdateId_;
    PerMeshObject perMeshObject_;
    PerObjectUpdate perObjectUpdateBuffer_;
    float rotationSpeed_;
    vec3 rotation_;
    vec3 scale_;
};
}  // namespace GameEngine
