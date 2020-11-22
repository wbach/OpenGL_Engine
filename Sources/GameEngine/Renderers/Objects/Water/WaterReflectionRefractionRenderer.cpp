#include "WaterReflectionRefractionRenderer.h"

#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

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

    if (not reflectionPerFrameBuffer_)
    {
        reflectionPerFrameBuffer_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));
    }

    if (not refractionPerFrameBuffer_)
    {
        refractionPerFrameBuffer_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));
    }
}
void WaterReflectionRefractionRenderer::prepare()
{
    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.EnableClipingPlane(0);

    createReflectionTexture();
    createRefractionTexture();

    context_.graphicsApi_.DisableCliping(0);
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
   // depthAttachment.compareMode = GraphicsApi::FrameBuffer::CompareMode::RefToTexture;

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

    auto& camera              = context_.scene_->GetCamera();
    float waterTilePositionY  = -10.f;
    auto projectionViewMatrix = context_.projection_.GetProjectionMatrix() * camera.GetViewMatrix();

    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.ProjectionViewMatrix = context_.graphicsApi_.PrepareMatrixToLoad(projectionViewMatrix);
    perFrameBuffer.cameraPosition       = camera.GetPosition();
    perFrameBuffer.clipPlane            = vec4(0.f, -1.f, 0.f, waterTilePositionY);

    context_.graphicsApi_.UpdateShaderBuffer(*refractionPerFrameBuffer_, &perFrameBuffer);
    auto lastBindedShaderBuffer = context_.graphicsApi_.BindShaderBuffer(*refractionPerFrameBuffer_);

    refractionFrameBuffer_->Clear();
    refractionFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    renderScene();
    refractionFrameBuffer_->UnBind();
    context_.graphicsApi_.BindShaderBuffer(lastBindedShaderBuffer);
}
Quaternion filpPitch(const Rotation& rotation)
{
    return Quaternion(rotation.value_.w, rotation.value_.z, -rotation.value_.y, -rotation.value_.x);
}
void WaterReflectionRefractionRenderer::createReflectionTexture()
{
    auto renderSize = EngineConf.renderer.water.waterReflectionResolution;
    context_.graphicsApi_.SetViewPort(0, 0, renderSize.x, renderSize.y);

    auto& camera = context_.scene_->GetCamera();

    float waterTilePositionY = -10.f;
    auto cameraPosition      = camera.GetPosition();

    float distance = 2.f * (cameraPosition.y - waterTilePositionY);
    cameraPosition.y -= distance;
    glm::vec3 newforward = glm::reflect(camera.GetDirection(), VECTOR_UP);

    auto rotation             = Utils::lookAt(newforward + cameraPosition, cameraPosition);
    auto viewMatrix           = Utils::createViewMatrix(rotation, cameraPosition);
    auto projectionViewMatrix = context_.projection_.GetProjectionMatrix() * viewMatrix;

    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.ProjectionViewMatrix = context_.graphicsApi_.PrepareMatrixToLoad(projectionViewMatrix);
    perFrameBuffer.cameraPosition       = cameraPosition;
    perFrameBuffer.clipPlane            = vec4(0.f, 1.f, 0.f, -waterTilePositionY);

    context_.graphicsApi_.UpdateShaderBuffer(*reflectionPerFrameBuffer_, &perFrameBuffer);
    auto lastBindedShaderBuffer = context_.graphicsApi_.BindShaderBuffer(*reflectionPerFrameBuffer_);

    reflectionFrameBuffer_->Clear();
    reflectionFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);

    renderScene();

    reflectionFrameBuffer_->UnBind();
    context_.graphicsApi_.BindShaderBuffer(lastBindedShaderBuffer);
}
}  // namespace GameEngine
