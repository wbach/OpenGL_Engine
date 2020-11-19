#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Renderers/IRenderer.h"
#include <Mutex.hpp>

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

    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void render() override;

private:
    void renderModel(const EntitySubscriber&, const Model&) const;
    void renderMesh(const Mesh&) const;
    void renderEntities();
    void bindMaterial(const Material&) const;
    void unBindMaterial(const Material&) const;
    void bindMaterialTexture(uint32, Texture*, bool) const;

private:
    RendererContext& context_;

    EnitySubscribers subscribes_;
    std::set<uint32> subscribesIds_;

    std::mutex subscriberMutex_;
};

}  // namespace GameEngine
