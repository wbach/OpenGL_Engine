#pragma once
#include <unordered_map>
#include "GameEngine/Renderers/Framebuffer/Postprocessing/PostproccesFrameBuffer.h"
#include "GameEngine/Renderers/Postproccesing/IPostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRendererTypes.h"
#include "Types.h"

namespace GameEngine
{
class Scene;

typedef std::vector<PostprocessingRendererPtr> PostprocessingRenderers;

class PostProcessingManager
{
public:
    PostProcessingManager(RendererContext& context);
    ~PostProcessingManager();
    void Init();
    void Render(const Scene&);
    void ReloadShaders();

private:
    void AddEffect(PostprocessingRendererType);
    void ResetBufferSet();
    void SwapBuffers();
    void BindBuffer(uint32 i);
    bool IsLastRenderer(uint32 i);
    void AddEffects();

private:
    RendererContext& context_;
    bool first_;
    PostprocessingRendererPtr ssaoRenderer_;
    PostprocessFrameBuffer* activePostProcessFrameBuffer_;
    PostprocessFrameBuffer* passivePostProcessFrameBuffer_;
    PostprocessFrameBuffer postproccesFrameBuffer1_;
    PostprocessFrameBuffer postproccesFrameBuffer2_;
    PostprocessFrameBuffer ambientOclusionFrameBuffer_;
    PostprocessingRenderers postProcessingRenderers_;
    std::unique_ptr<IPostprocessingRenderersFactory> factory_;
};
}  // namespace GameEngine
