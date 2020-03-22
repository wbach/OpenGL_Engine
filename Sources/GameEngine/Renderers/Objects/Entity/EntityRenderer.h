#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class Entity;
class Projection;
class ModelWrapper;
struct RendererContext;
struct Material;

namespace Components
{
class RendererComponent;
}  // namespace Components

struct EntitySubscriber
{
    GameObject* gameObject;
    Components::RendererComponent* renderComponent;
};

typedef std::vector<EntitySubscriber> EnitySubscribers;

class EntityRenderer : public IRenderer
{
public:
    EntityRenderer(RendererContext& context);
    ~EntityRenderer();

    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(const Scene& scene, const Time& threadTime);

private:
    void RenderModel(const EntitySubscriber& subsriber, const Model& model) const;
    void RenderMesh(const Mesh& mesh) const;
    void RenderEntities();
    void BindMaterial(const Material& material) const;
    void UnBindMaterial(const Material& material) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;

    EnitySubscribers subscribes_;

    GraphicsApi::ID perObjectUpdateId;
    GraphicsApi::ID perObjectId;
};

}  // namespace GameEngine
