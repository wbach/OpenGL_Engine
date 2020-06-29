#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

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
    Components::RendererComponent* renderComponent{nullptr};
    Components::Animator* animator{nullptr};
};

typedef std::vector<EntitySubscriber> EnitySubscribers;

class EntityRenderer : public IRenderer
{
public:
    EntityRenderer(RendererContext&);
    ~EntityRenderer();

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    void RenderModel(const EntitySubscriber&, const Model&) const;
    void RenderMesh(const Mesh&) const;
    void RenderEntities();
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, Texture*, bool) const;
    void bindShadowMap(uint32 id, uint32 nr) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;

    EnitySubscribers subscribes_;
    std::set<uint32> subscribesIds_;
};

}  // namespace GameEngine
