#pragma once
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"

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

class TerrainMeshRenderer : public IRenderer
{
    struct Subscriber
    {
        GameObject* gameObject_;
        Components::TerrainMeshRendererComponent* component_;
    };

public:
    TerrainMeshRenderer(RendererContext& context);
    ~TerrainMeshRenderer();

    virtual void Init() override;
    virtual void Subscribe(GameObject*) override;
    virtual void UnSubscribe(GameObject*) override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene&, const Time&);

private:
    void BindTexture(Texture*, int id) const;
    void BindTextures(const TerrainTexturesMap&) const;
    void RenderSubscribers(const Scene&) const;
    void RenderSubscriber(const Scene&, const Subscriber&) const;
    void RenderMesh(const Mesh&, const GraphicsApi::ID&) const;
    void PartialRendering(const Scene&, const Subscriber&) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    std::vector<std::pair<uint32_t, Subscriber>> subscribes_;
};
}  // namespace GameEngine
