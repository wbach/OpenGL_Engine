#pragma once
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Shaders/ShaderProgram.h"

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
} // namespace Components

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
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time&);

private:
    void BindTexture(Texture* texture, int id) const;
    void BindTextures(const TerrainTexturesMap& textures) const;
    void RenderSubscribers() const;
    void RenderSubscriber(const Subscriber& subscriber) const;
    void RenderMesh(const Mesh& mesh) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    std::vector<std::pair<uint32_t, Subscriber>> subscribes_;
};
}  // namespace GameEngine
