#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Objects/Tree/ShadowMapTreeRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShadowsBuffer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "ShadowBox.h"

namespace GameEngine
{
class Mesh;
struct Material;
class Projection;
class ShadowFrameBuffer;
class ModelWrapper;
struct Time;

namespace Components
{
class Animator;
class RendererComponent;
}  // namespace Components

class ShadowMapRenderer : public IRenderer
{
public:
    ShadowMapRenderer(RendererContext&);
    ~ShadowMapRenderer();
    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void prepare() override;

private:
    void cleanUp() override;
    void renderScene();
    bool isInit() const;
    bool prepareFrameBuffer();
    void renderCascades();
    mat4 convertNdcToTextureCooridates(const mat4&) const;

private:
    RendererContext& context_;
    EntityRenderer entityRenderer_;
    ShadowMapTreeRenderer treeRenderer_;

    ShaderProgram shader_;
    ShaderProgram instancedShader_;

    ShadowBox shadowBox_;
    mat4 projectionViewMatrix_;
    mat4 biasMatrix_;
    ShadowsBuffer buffer_;
    GraphicsApi::ID perFrameBuffer_;
    GraphicsApi::IFrameBuffer* shadowFrameBuffer_[Params::MAX_SHADOW_MAP_CASADES];

    bool isActive_;
    IdType shadowEnabledSubscriptionId_;
    MeasurementValue* measurementValue_;
    uint32 rendererdMeshesCounter_;
    GraphicsApi::ID shadowsBufferId_;
};
}  // namespace GameEngine
