#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ShaderBuffers/ShadowsBuffer.h"
#include "ShadowBox.h"

namespace GameEngine
{
class Mesh;
class CameraWrapper;
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
    void renderScene();
    bool isInit() const;
    void prepareFrameBuffer();
    void renderCascades();
    mat4 convertNdcToTextureCooridates(const mat4&) const;

private:
    RendererContext& context_;
    EntityRenderer entityRenderer_;

    ShaderProgram shader_;
    ShadowBox shadowBox_;
    mat4 projectionViewMatrix_;
    mat4 biasMatrix_;
    ShadowsBuffer buffer_;
    GraphicsApi::ID perFrameBuffer_;
    GraphicsApi::IFrameBuffer* shadowFrameBuffer_[Params::MAX_SHADOW_MAP_CASADES];
};
}  // namespace GameEngine
