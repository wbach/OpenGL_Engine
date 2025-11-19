#pragma once
#include <unordered_map>

#include "GameEngine/Renderers/Postproccesing/IPostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "Logger/Log.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRendererTypes.h"
#include "Types.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
class Scene;

typedef std::vector<PostprocessingRendererPtr> PostprocessingRenderers;

inline GraphicsApi::FrameBuffer::Attachment createColorAttachment(const vec2ui& size)
{
    GraphicsApi::FrameBuffer::Attachment colorAttachment;
    colorAttachment.size   = size;
    colorAttachment.type   = GraphicsApi::FrameBuffer::Type::Color0;
    colorAttachment.format = GraphicsApi::FrameBuffer::Format::Rgba32f;
    colorAttachment.filter = GraphicsApi::FrameBuffer::Filter::Linear;
    return colorAttachment;
}

class FrameBuffersManager
{
public:
    FrameBuffersManager(GraphicsApi::IGraphicsApi& graphicsApi, const vec2ui& size)
        : graphicsApi_(graphicsApi)
        , fboA{graphicsApi.CreateFrameBuffer({createColorAttachment(size)})}
        , fboB{graphicsApi.CreateFrameBuffer({createColorAttachment(size)})}
        , selectedA{true}
        , status_{fboA.Init() and fboB.Init()}
    {
        LOG_DEBUG << "Postprocessing buffer size: " << size;
        LOG_DEBUG << "FrameBuffersManager, fboA id = " << fboA.GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
        LOG_DEBUG << "FrameBuffersManager, fboB id = " << fboB.GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
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
    void OnSizeChanged();
    void Render(GraphicsApi::IFrameBuffer&, const Scene&);
    void ReloadShaders();

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
