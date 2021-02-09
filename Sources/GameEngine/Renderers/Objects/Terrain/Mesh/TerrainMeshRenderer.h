#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"

namespace GameEngine
{
class Model;
class Mesh;
class Texture;
class Projection;
class TerrainWrapper;
struct RendererContext;

typedef TerrainWrapper* TerrainPtr;

namespace Components
{
class TerrainMeshRendererComponent;
}  // namespace Components

class TerrainMeshRenderer
{
    struct Subscriber
    {
        GameObject* gameObject_{nullptr};
        Components::TerrainMeshRendererComponent* component_{nullptr};
    };

public:
    TerrainMeshRenderer(RendererContext& context);
    ~TerrainMeshRenderer();

    void subscribe(GameObject&);
    void unSubscribe(GameObject&);
    void unSubscribeAll();
    uint32 renderSubscribers();

private:
    void bindTexture(Texture*, uint32 id) const;
    void bindTextures(const std::vector<std::pair<TerrainTextureType, Texture*>>&) const;
    void renderSubscriber(const Subscriber&);
    void renderMesh(const Mesh&, const GraphicsApi::ID&);
    void renderTerrainMeshes(const Model&, const Components::TerrainMeshRendererComponent&);

private:
    RendererContext& context_;
    std::vector<std::pair<uint32_t, Subscriber>> subscribes_;
    std::mutex subscriberMutex_;
    uint32 renderedTerrains_;
};
}  // namespace GameEngine
