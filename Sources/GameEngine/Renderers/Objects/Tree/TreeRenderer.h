#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Mesh;
class Model;
struct Material;
struct RendererContext;
class ModelWrapper;
struct Time;
class GeneralTexture;

namespace Components
{
class TreeRendererComponent;
}  // namespace Components

struct TreeSubscriber
{
    GameObject* gameObject_{nullptr};
    Components::TreeRendererComponent* treeRendererComponent_{nullptr};
};

class TreeRenderer : public IRenderer
{
    typedef std::vector<TreeSubscriber> Subscribers;

public:
    TreeRenderer(RendererContext&);

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    void RenderModel(const Model&, const TreeSubscriber&) const;
    void RenderMesh(const Mesh&, uint32) const;
    void RenderTrees();
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, GeneralTexture*, bool) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;

    Subscribers subscribes_;
};
}  // namespace GameEngine
