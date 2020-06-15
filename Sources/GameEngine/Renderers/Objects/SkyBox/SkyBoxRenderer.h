#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"

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
    Model& model_;
    Texture* dayTexture_;
    Texture* nightTexture_;
};

typedef std::unordered_map<uint32_t, SkyBoxSubscriber> SkyBoxSubscriberMap;

class SkyBoxRenderer : public IRenderer
{
public:
    SkyBoxRenderer(RendererContext&);
    void init() override;
    void subscribe(GameObject&) override;
    void reloadShaders() override;
    void render();

private:
    void BindTextures(const SkyBoxSubscriber& sub) const;
    void RenderSkyBoxMesh(const Mesh& mesh) const;
    void RenderSkyBoxModel(const SkyBoxSubscriber& sub);
    void prepareShaderBeforeFrameRender();
    void BindCubeMapTexture(const Texture& texture, uint32 id) const;
    void prepareToRendering();
    void updateBuffer();

private:
    struct PerMeshObject
    {
        AlignWrapper<vec4> fogColor_;
        AlignWrapper<float> blendFactor_;
    };

private:
    RendererContext& context_;
    ShaderProgram shader_;
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
