#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
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
    void Render(Scene* scene);
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void BindTextures(const SkyBoxSubscriber& sub) const;
    void RenderSkyBoxMesh(const Mesh& mesh) const;
    void RenderSkyBoxModel(const SkyBoxSubscriber& sub);
    void PrepareShaderBeforeFrameRender(Scene* scene);
    void BindCubeMapTexture(Texture* texture, int id) const;
    void PrepareToRendering(Scene* scene);
    void EndRendering();
    void InitShader();
    mat4 ModifiedViewMatrix(const mat4& viewMatrix) const;

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    vec4 clipPlane;
    SkyBoxSubscriberMap subscribes_;
    float rotation_;
};
}  // GameEngine
