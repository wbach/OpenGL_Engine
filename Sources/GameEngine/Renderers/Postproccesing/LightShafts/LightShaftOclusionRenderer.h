#pragma once
#include <optional>

#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class DirectionalLightComponent;
}
class LightShaftOclusionRenderer
{
public:
    LightShaftOclusionRenderer(RendererContext&);
    ~LightShaftOclusionRenderer();

    void Init();
    void CleanUp();
    void Render(uint32);
    void ReloadShaders();

    vec4 calculateSunPosition(const Components::DirectionalLightComponent&);

private:
    void Render(ShaderProgram&, GraphicsApi::IFrameBuffer&, uint32);
    GraphicsApi::IFrameBuffer* createFrameBuffer();

private:
    RendererContext& context;

    ShaderProgram oclusionShader;
    ShaderProgram blurShader;
    GraphicsApi::IFrameBuffer* oclusionFrameBuffer{nullptr};
    GraphicsApi::IFrameBuffer* blurFrameBuffer{nullptr};
    std::optional<vec2ui> frameBufferSize;
    std::optional<IdType> sunBufferId;
    std::optional<IdType> oclusionTextureId;
};
}  // namespace GameEngine
