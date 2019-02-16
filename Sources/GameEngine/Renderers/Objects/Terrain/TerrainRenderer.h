#pragma once
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
class Model;
class Texture;
class Projection;
class TerrainWrapper;
struct RendererContext;
class IShaderProgram;

typedef TerrainWrapper* TerrainPtr;

class TerrainRenderer : public IRenderer
{
public:
    TerrainRenderer(RendererContext& context);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time& threadTime);

private:
    void BindTexture(Texture* texture, int id) const;
    void BindTextures(const TerrainTexturesMap& textures) const;
    void RenderSubscribers(const mat4& viewMatrix) const;
    void RenderSubscriber(const TerrainTexturesMap& textures) const;
    void InitShader();

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    vec4 clipPlane;
    std::vector<std::pair<uint32_t, Components::TerrainRendererComponent*>> subscribes_;
    uint32 objectId;
};
}  // namespace GameEngine
