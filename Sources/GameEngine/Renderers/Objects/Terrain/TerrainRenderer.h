#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
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
    TerrainRenderer(RendererContext&);
    ~TerrainRenderer();

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:

private:
    bool IsInit() const;
    void BindTexture(Texture&, uint32 id) const;
    void BindTextures(const std::vector<std::pair<TerrainTextureType, Texture*>>& textures) const;
    void RenderSubscribers() const;
    void RenderSubscriber(const std::vector<std::pair<TerrainTextureType, Texture*>>& textures) const;
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
