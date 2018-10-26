#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Shaders/ShadowShader.h"
#include "ShadowBox.h"

namespace GameEngine
{
class Mesh;
struct ICamera;
struct Material;
class Projection;
class ShadowFrameBuffer;
class ModelWrapper;

class ShadowMapRenderer : public IRenderer
{
    struct ShadowMapSubscriber
    {
        uint32 textureIndex    = 0;
        GameObject* gameObject = nullptr;
        ModelWrapper* model    = nullptr;
    };
    typedef std::unordered_map<uint32_t, ShadowMapSubscriber> SubscribersMap;

public:
    ShadowMapRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void PrepareRender(Scene*);
    void PrepareShader(ICamera*) const;
    void RenderSubscribes() const;
    void RenderSubscriber(const ShadowMapSubscriber&) const;
    void RenderMesh(const Mesh& mesh, const mat4& transform_matrix, uint32 textureIndex) const;
    void BindMaterial(const Material&, uint32 textureIndex) const;

private:
    RendererContext& context_;
    ShadowShader shader_;
    ShadowBox shadowBox_;
    ShadowBox shadowBox2_;
    mat4 projectionViewMatrix_;
    mat4 viewOffset_;
    SubscribersMap subscribes_;
};
}  // GameEngine
