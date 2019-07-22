#pragma once
#include <memory>
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "Types.h"

namespace GameEngine
{
class Texture;
class IShaderProgram;
struct RendererContext;

class TerrainNormalMapRenderer : public IRenderer
{
public:
    TerrainNormalMapRenderer(RendererContext&);
    virtual void Render(const Scene&, const Time&);
    virtual void Init() override;
    virtual void Subscribe(GameObject*) override;
    virtual void UnSubscribe(GameObject*) override;
    virtual void UnSubscribeAll();
    virtual void ReloadShaders() override;

private:
    std::unique_ptr<Texture> RenderTexture(const Texture&) const;
    std::unique_ptr<Texture> RenderOnCpu(const Texture&);

private:
    std::vector<Components::TerrainRendererComponent*> subscribers_;
    std::unique_ptr<IShaderProgram> shader_;
    RendererContext& context_;
    bool useShader_;
};
}  // namespace GameEngine
