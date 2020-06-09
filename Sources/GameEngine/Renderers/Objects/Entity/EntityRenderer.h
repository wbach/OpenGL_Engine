#pragma once
#include <set>

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
class Texture;
struct RendererContext;
struct Material;

namespace Components
{
class Animator;
class RendererComponent;
}  // namespace Components

struct EntitySubscriber
{
    GameObject* gameObject{nullptr};
    Components::Animator* animator{nullptr};
    Components::RendererComponent* renderComponent{nullptr};
};

typedef std::vector<EntitySubscriber> EnitySubscribers;

class EntityRenderer : public IRenderer
{
public:
    EntityRenderer(RendererContext& context);
    ~EntityRenderer();

    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject*) override;
    virtual void UnSubscribe(GameObject*) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(const Scene&, const Time&);

private:
    void RenderModel(const EntitySubscriber&, const Model&) const;
    void RenderMesh(const Mesh&) const;
    void RenderEntities();
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, Texture*, bool) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;

    EnitySubscribers subscribes_;
    std::set<uint32> subscribesIds_;
};

}  // namespace GameEngine
