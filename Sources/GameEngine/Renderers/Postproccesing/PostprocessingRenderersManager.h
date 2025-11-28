#pragma once
#include <Types.h>

#include "PostprocessingRenderer.h"
#include "PostprocessingRendererTypes.h"

namespace GraphicsApi
{
class IFrameBuffer;
}  // namespace GraphicsApi

namespace GameEngine
{
class Scene;
class FrameBuffersManager;
struct RendererContext;
class IPostprocessingRenderersFactory;

typedef std::vector<PostprocessingRendererPtr> PostprocessingRenderers;

class PostProcessingManager
{
public:
    PostProcessingManager(RendererContext&);
    ~PostProcessingManager();

    void Init();
    void OnSizeChanged();
    void Render(GraphicsApi::IFrameBuffer&, GraphicsApi::IFrameBuffer*, const Scene&);
    void ReloadShaders();
    void CleanUp();

private:
    void AddEffect(PostprocessingRendererType);
    bool IsLastRenderer(size_t i);
    void AddEffects();
    void bindDefaultFrameBuffer();

private:
    RendererContext& context_;
    std::unique_ptr<FrameBuffersManager> fboManager_;
    PostprocessingRenderers postProcessingRenderers_;
    std::unique_ptr<IPostprocessingRenderersFactory> factory_;
};
}  // namespace GameEngine
