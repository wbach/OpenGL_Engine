#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/ShaderProgram.h"
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
    bool IsInit() const;
    void prepareRender();
    void RenderSubscribes() const;
    void RenderSubscriber(const ShadowMapSubscriber&) const;
    void RenderMesh(const Mesh& mesh) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    ShadowBox shadowBox_;
    mat4 projectionViewMatrix_;
    mat4 viewOffset_;
    Subscribers subscribes_;
    GraphicsApi::ID perFrameBuffer_;
    GraphicsApi::IFrameBuffer* shadowFrameBuffer_;
};
}  // namespace GameEngine
