#pragma once
#include <unordered_map>
#include "GameEngine/Renderers/Postproccesing/IPostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRendererTypes.h"
#include "Types.h"

namespace GameEngine
{
class Scene;

typedef std::vector<PostprocessingRendererPtr> PostprocessingRenderers;

class FrameBuffersManager
{
public:
    FrameBuffersManager(GraphicsApi::IGraphicsApi& graphicsApi, GraphicsApi::FrameBuffer::Attachment attachment)
        : graphicsApi_(graphicsApi)
        , fboA{graphicsApi.CreateFrameBuffer({attachment})}
        , fboB{graphicsApi.CreateFrameBuffer({attachment})}
        , selectedA{true}
        , status_{fboA.Init() and fboB.Init()}
    {
    }
    ~FrameBuffersManager()
    {
        graphicsApi_.DeleteFrameBuffer(fboA);
        graphicsApi_.DeleteFrameBuffer(fboB);
    }
    bool GetStatus() const
    {
        return status_;
    }
    void BindForWriting()
    {
        auto mode = GraphicsApi::FrameBuffer::BindType::Write;
        selectedA ? fboA.Bind(mode) : fboB.Bind(mode);
    }
    void BindForReading()
    {
        auto mode = GraphicsApi::FrameBuffer::BindType::Textures;
        selectedA ? fboB.Bind(mode) : fboA.Bind(mode);
    }
    void StartFrame()
    {
        selectedA = true;
    }
    void Swap()
    {
        selectedA = !selectedA;
    }

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::IFrameBuffer& fboA;
    GraphicsApi::IFrameBuffer& fboB;
    bool selectedA{true};
    bool status_;
};

class PostProcessingManager
{
public:
    PostProcessingManager(RendererContext& context);
    ~PostProcessingManager();
    void Init();
    void Render(GraphicsApi::IFrameBuffer&, const Scene&);
    void ReloadShaders();

private:
    void AddEffect(PostprocessingRendererType);
    bool IsLastRenderer(size_t i);
    void AddEffects();
    void bindDefaultFrameBuffer();

private:
    RendererContext& context_;
    bool first_;
    std::unique_ptr<FrameBuffersManager> fboManager_;
    PostprocessingRenderers postProcessingRenderers_;
    std::unique_ptr<IPostprocessingRenderersFactory> factory_;
};
}  // namespace GameEngine
