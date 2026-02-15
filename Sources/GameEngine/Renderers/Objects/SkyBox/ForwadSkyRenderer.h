#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include <mutex>
#include <optional>

#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class ForwadSkyRenderer : public IRenderer
{
public:
    ForwadSkyRenderer(RendererContext&);

    void init() override;
    void reloadShaders() override;
    void render() override;

private:
    void updateBuffer();

private:
    RendererContext& context_;
    ShaderProgram shader_;

    std::optional<IdType> bufferId;
};
}  // namespace GameEngine
