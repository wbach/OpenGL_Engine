#include "WaterReflectionRefractionRenderer.h"

namespace GameEngine
{
WaterReflectionRefractionRenderer::WaterReflectionRefractionRenderer(RendererContext& context)
    : context_(context)
    , entityRenderer_(context)
    , terrainMeshRenderer_(context)
    , skyBoxRenderer_(context)
    , entityShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SimpleForwadEntity)
    , terrainShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SimpleForwardTerrainMesh)
    , skyBoxShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::ForwardSkyBox)
{
}
WaterReflectionRefractionRenderer::~WaterReflectionRefractionRenderer()
{
}
void WaterReflectionRefractionRenderer::init()
{
    entityShader_.Init();
    terrainShader_.Init();
    skyBoxShader_.Init();
    skyBoxRenderer_.init();

    reflectionFrameBuffer_ = createWaterFbo(EngineConf.renderer.water.waterReflectionResolution);
    refractionFrameBuffer_ = createWaterFbo(EngineConf.renderer.water.waterRefractionResolution);

    if (not context_.waterReflectionTextureId_ and reflectionFrameBuffer_)
    {
        context_.waterReflectionTextureId_ =
            reflectionFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
    }

    if (refractionFrameBuffer_)
    {
        if (not context_.waterRefractionTextureId_)
        {
            context_.waterRefractionTextureId_ =
                refractionFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
        }
        if (not context_.waterRefractionDepthTextureId_)
        {
            context_.waterRefractionDepthTextureId_ =
                refractionFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth);
        }
    }
}
void WaterReflectionRefractionRenderer::prepare()
{
    context_.graphicsApi_.EnableDepthTest();

    createReflectionTexture();
    createRefractionTexture();

    const auto& renderingSize = context_.projection_.GetRenderingSize();
    context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
}
void WaterReflectionRefractionRenderer::subscribe(GameObject& gameObject)
{
    skyBoxRenderer_.subscribe(gameObject);
    entityRenderer_.subscribe(gameObject);
    terrainMeshRenderer_.subscribe(gameObject);
}
void WaterReflectionRefractionRenderer::unSubscribe(GameObject& gameObject)
{
    skyBoxRenderer_.unSubscribe(gameObject);
    entityRenderer_.unSubscribe(gameObject);
    terrainMeshRenderer_.unSubscribe(gameObject);
}
void WaterReflectionRefractionRenderer::unSubscribeAll()
{
    skyBoxRenderer_.unSubscribeAll();
    entityRenderer_.unSubscribeAll();
    terrainMeshRenderer_.unSubscribeAll();
}
void WaterReflectionRefractionRenderer::reloadShaders()
{
    skyBoxShader_.Reload();
    entityShader_.Reload();
    terrainShader_.Reload();
}
GraphicsApi::IFrameBuffer* WaterReflectionRefractionRenderer::createWaterFbo(const vec2ui& size)
{
    GraphicsApi::FrameBuffer::Attachment depthAttachment(size, GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode    = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter      = GraphicsApi::FrameBuffer::Filter::Linear;
    depthAttachment.compareMode = GraphicsApi::FrameBuffer::CompareMode::RefToTexture;

    GraphicsApi::FrameBuffer::Attachment colorAttachment(size, GraphicsApi::FrameBuffer::Type::Color0,
                                                         GraphicsApi::FrameBuffer::Format::Rgba8);

    colorAttachment.filter = GraphicsApi::FrameBuffer::Filter::Linear;

    auto result = &context_.graphicsApi_.CreateFrameBuffer({depthAttachment, colorAttachment});

    auto status = result->Init();

    if (not status)
    {
        context_.graphicsApi_.DeleteFrameBuffer(*result);
        ERROR_LOG("Shadow framebuffer creation error.");
        return nullptr;
    }
    return result;
}
void WaterReflectionRefractionRenderer::renderScene()
{
    skyBoxShader_.Start();
    skyBoxRenderer_.render();

    entityShader_.Start();
    entityRenderer_.render();

    terrainShader_.Start();
    terrainMeshRenderer_.renderSubscribers();
}
void WaterReflectionRefractionRenderer::createRefractionTexture()
{
    auto renderSize = EngineConf.renderer.water.waterRefractionResolution;
    context_.graphicsApi_.SetViewPort(0, 0, renderSize.x, renderSize.y);

    refractionFrameBuffer_->Clear();
    refractionFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);

    renderScene();
    refractionFrameBuffer_->UnBind();
}
void WaterReflectionRefractionRenderer::createReflectionTexture()
{
    auto renderSize = EngineConf.renderer.water.waterReflectionResolution;
    context_.graphicsApi_.SetViewPort(0, 0, renderSize.x, renderSize.y);

    reflectionFrameBuffer_->Clear();
    reflectionFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);

    renderScene();
    reflectionFrameBuffer_->UnBind();
}
}  // namespace GameEngine
