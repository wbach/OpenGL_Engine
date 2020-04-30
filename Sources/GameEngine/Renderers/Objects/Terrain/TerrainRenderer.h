#pragma once
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Model;
class Texture;
class Projection;
class TerrainWrapper;
struct RendererContext;
class TerrainNode;

typedef TerrainWrapper* TerrainPtr;

class TerrainRenderer : public IRenderer
{
public:
    TerrainRenderer(RendererContext& context);
    ~TerrainRenderer();
    // Loading lights itp to shader
    void Init() override;
    void Subscribe(GameObject* gameObject) override;
    void UnSubscribe(GameObject* gameObject) override;
    void UnSubscribeAll();
    void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time& threadTime);

private:
    bool IsInit() const;
    void BindTexture(Texture &, uint32 id) const;
    void BindTextures(const TerrainTexturesMap& textures) const;
    void RenderSubscribers() const;
    void RenderSubscriber(const TerrainTexturesMap& textures) const;
    void RenderNode(const TerrainNode& node) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    vec4 clipPlane;
    std::vector<std::pair<uint32_t, Components::TerrainTessellationRendererComponent*>> subscribes_;
    GraphicsApi::ID objectId;
    GraphicsApi::ID perTerrainId;
    GraphicsApi::ID perNodeId;
};
}  // namespace GameEngine
