#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

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

class SkyBoxRenderer
{
public:
    SkyBoxRenderer(RendererContext&);
    ~SkyBoxRenderer();

    void init();
    void subscribe(GameObject&);
    void unSubscribe(GameObject&);
    void unSubscribeAll();
    void render();
    void cleanUp();

private:
    void BindTextures(const SkyBoxSubscriber& sub) const;
    void RenderSkyBoxMesh(const Mesh& mesh) const;
    void RenderSkyBoxModel(const SkyBoxSubscriber& sub);
    void prepareShaderBeforeFrameRender();
    void BindCubeMapTexture(const Texture& texture, uint32 id) const;
    void prepareToRendering();
    void updateBuffer();

private:
    void calculateBoxScale();

    struct PerMeshObject
    {
        AlignWrapper<float> blendFactor_;
    };

private:
    RendererContext& context_;
    SkyBoxSubscriberMap subscribes_;
    std::mutex subscriberMutex_;
    GraphicsApi::ID perMeshObjectId_;
    GraphicsApi::ID perObjectUpdateId_;
    PerMeshObject perMeshObject_;
    PerObjectUpdate perObjectUpdateBuffer_;
    float rotationSpeed_;
    vec3 rotation_;
    vec3 scale_;
    IdType viewDistanceChangeSubscription_;
};
}  // namespace GameEngine
