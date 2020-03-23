#pragma once
#include <memory>
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GameEngine/Time/Time.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class TerrainTessellationRendererComponent;
} // namespace Components

class Texture;
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
    std::vector<Components::TerrainTessellationRendererComponent*> subscribers_;
    RendererContext& context_;
    ShaderProgram shader_;
};
}  // namespace GameEngine
