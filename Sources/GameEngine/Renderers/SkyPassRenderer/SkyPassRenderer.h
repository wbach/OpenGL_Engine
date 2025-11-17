#pragma once
#include <optional>
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
class SkyPassRenderer
{
public:
    SkyPassRenderer(RendererContext&);
    void Init();
    void Prepare();
    void Render(uint32);
    void ReloadShaders();

private:
    RendererContext& context;
    ShaderProgram shader;
    GraphicsApi::IFrameBuffer* frameBuffer;
    std::optional<vec2ui> frameBufferSize;
    bool isReady{false};
    std::optional<IdType> bufferId;

};
}  // namespace GameEngine
