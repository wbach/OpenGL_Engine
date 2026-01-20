#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <optional>
#include <vector>

#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/ShaderProgramType.h"
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

    virtual void RenderTree(const Components::TreeRendererComponent&, float) = 0;

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void render() override;

protected:
    float getDistanceToCamera(Components::IComponent&) const;

    void RenderModel(const Model&) const;
    void RenderMesh(const Mesh&) const;

    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, GeneralTexture*, bool) const;
    void UpdateTreePramBuffer(Components::TreeRendererComponent&, float);
    float CalculateFadeFactor(float);

protected:
    RendererContext& context_;
    Subscribers subscribes_;
    MeasurementValue& measurementValue_;
    std::optional<IdType> paramBufferId_;

    float startFade = 20.0;
    float endFade   = 30.0;
};
}  // namespace GameEngine
