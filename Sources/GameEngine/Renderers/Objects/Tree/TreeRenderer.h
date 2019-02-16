#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
class Mesh;
class Model;
struct Material;
struct RendererContext;
class ModelWrapper;
class IShaderFactory;
class IShaderProgram;
struct Time;

namespace Components
{
class TreeRendererComponent;
}  // namespace Components

struct TreeSubscriber
{
    GameObject* gameObject_;
    Components::TreeRendererComponent* treeRendererComponent_;
};

class TreeRenderer : public IRenderer
{
    typedef std::vector<TreeSubscriber> Subscribers;

public:
    TreeRenderer(RendererContext& context);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time& threadTime);

private:
    void RenderModel(const Model& model, const TreeSubscriber&) const;
    void RenderMesh(const Mesh& mesh, uint32) const;
    void RenderTrees();
    void BindMaterial(const Material& material) const;
    void UnBindMaterial(const Material& material) const;

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;

    vec4 clipPlane;

    Subscribers subscribes_;
};
}  // namespace GameEngine
