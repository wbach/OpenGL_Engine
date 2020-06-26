#pragma once
#include <GraphicsApi/IGraphicsApi.h>

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
    struct ShadowMapSubscriber
    {
        GameObject* gameObject{nullptr};
        Components::RendererComponent* renderComponent{nullptr};
        Components::Animator* animator{nullptr};
    };
    typedef std::vector<ShadowMapSubscriber> Subscribers;

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
    bool isInit() const;
    void prepareFrameBuffer();
    void renderCascades() const;
    void renderSubscribes() const;
    void renderSubscriber(const ShadowMapSubscriber&) const;
    void renderMesh(const Mesh& mesh) const;
    mat4 convertNdcToTextureCooridates(const mat4&) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    ShadowBox shadowBox_;
    mat4 projectionViewMatrix_;
    mat4 biasMatrix_;
    ShadowsBuffer buffer_;
    Subscribers subscribes_;
    GraphicsApi::ID perFrameBuffer_;
    GraphicsApi::IFrameBuffer* shadowFrameBuffer_[Params::MAX_SHADOW_MAP_CASADES];
};
}  // namespace GameEngine
