#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <optional>
#include <vector>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Types.h"

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
    std::vector<Components::TreeRendererComponent*> treeRendererComponent_{nullptr};
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
    int RenderSingleTree(const Components::TreeRendererComponent&);
    int RenderInstancedTree(const Components::TreeRendererComponent&);

    int RenderModel(const Model&) const;
    int RenderModel(const Model&, uint32) const;
    int RenderMesh(const Mesh&) const;
    int RenderMesh(const Mesh&, uint32) const;
    void RenderLeafs(const Components::TreeRendererComponent&) const;
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, GeneralTexture*, bool) const;

private:
    RendererContext& context_;
    ShaderProgram leafsShader_;
    ShaderProgram trunkShader_;

    Subscribers subscribes_;
    MeasurementValue& measurementValue_;

    std::optional<IdType> paramBufferId_;
};
}  // namespace GameEngine
