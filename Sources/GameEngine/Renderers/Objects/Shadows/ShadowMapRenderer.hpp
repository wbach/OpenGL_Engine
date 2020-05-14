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
class RendererComponent;
}  // namespace Components

class ShadowMapRenderer : public IRenderer
{
    struct ShadowMapSubscriber
    {
        GameObject* gameObject                         = nullptr;
        Components::RendererComponent* renderComponent = nullptr;
    };
    typedef std::vector<ShadowMapSubscriber> Subscribers;

public:
    ShadowMapRenderer(RendererContext& context);
    ~ShadowMapRenderer();
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    bool IsInit() const;
    void Render(const Scene&, const Time&);
    void PrepareRender(const Scene&);
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
