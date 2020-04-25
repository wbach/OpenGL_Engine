#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Mesh;
class Model;
struct Material;
struct RendererContext;
class ModelWrapper;
struct Time;
class Texture;

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
    TreeRenderer(RendererContext&);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject*) override;
    virtual void UnSubscribe(GameObject*) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene&, const Time&);

private:
    void RenderModel(const Model&, const TreeSubscriber&) const;
    void RenderMesh(const Mesh&, uint32) const;
    void RenderTrees();
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, Texture*, bool) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;

    vec4 clipPlane;

    Subscribers subscribes_;
};
}  // namespace GameEngine
