#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <Mutex.hpp>
#include <unordered_map>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class Entity;
class Projection;
class ModelWrapper;
class GeneralTexture;
struct RendererContext;
struct Material;
class ShaderProgram;

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

class EntityRenderer
{
    struct GroupedEntities
    {
        std::unordered_map<Model*, std::vector<const EntitySubscriber*>> singleEntitiesToRender_;
        std::unordered_map<Model*, std::vector<std::vector<const EntitySubscriber*>>> groupsToRender_;
    };

public:
    EntityRenderer(RendererContext&);
    virtual ~EntityRenderer();

    void init();
    void cleanUp();
    void subscribe(GameObject&);
    void unSubscribe(GameObject&);
    void unSubscribe(const Components::IComponent&);
    void unSubscribeAll();

    uint32 renderEntitiesWithoutGrouping();
    uint32 renderEntityWithGrouping(ShaderProgram&, ShaderProgram&);

private:
    GroupedEntities groupEntities() const;

    void renderModel(const EntitySubscriber&, const Model&);
    void renderMesh(const Mesh&,const Material&);
    void bindMaterial(const Material&) const;
    void unBindMaterial(const Material&) const;
    void bindMaterialTexture(uint32, GeneralTexture*, bool) const;
    const std::pair<GraphicsApi::ID, const Material*> getMaterial(const EntitySubscriber&, const Mesh&);

private:
    RendererContext& context_;

    std::unique_ptr<ShaderBufferObject<PerObjectUpdate>> perMeshBuffer_;
    std::unique_ptr<ShaderBufferObject<PerInstances>> perInstanceBuffer_;

    EnitySubscribers subscribes_;
    std::set<uint32> subscribesIds_;

    std::mutex subscriberMutex_;

    uint32 renderedMeshes_;
};

}  // namespace GameEngine
