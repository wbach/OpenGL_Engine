#include "WaterReflectionRefractionRenderer.h"

#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace
{
const float POSITION_EPSILON = 0.01f;
}
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

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    for (auto& subscriber : subscribers_)
    {
        auto fbo = getFbo(subscriber.first, subscriber.second);

        if (fbo)
        {
            createReflectionTexture(*fbo);
            createRefractionTexture(*fbo);
            subscriber.second.waterTextures_ = &fbo->waterTextures_;
        }

        cleanNotUsedFbos();
    }

    context_.graphicsApi_.DisableCliping(0);
    const auto& renderingSize = context_.projection_.GetRenderingSize();
    context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
}
void WaterReflectionRefractionRenderer::subscribe(GameObject& gameObject)
{
    skyBoxRenderer_.subscribe(gameObject);
    entityRenderer_.subscribe(gameObject);
    terrainMeshRenderer_.subscribe(gameObject);

    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    if (waterComponent)
    {
        subscribers_.insert({gameObject.GetId(), {gameObject, nullptr, std::nullopt}});
    }
}
void WaterReflectionRefractionRenderer::unSubscribe(GameObject& gameObject)
{
    skyBoxRenderer_.unSubscribe(gameObject);
    entityRenderer_.unSubscribe(gameObject);
    terrainMeshRenderer_.unSubscribe(gameObject);

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribers_.erase(gameObject.GetId());
    auto fbo = findFbo(gameObject.GetWorldTransform().GetPosition().y);

    if (fbo)
    {
        fbo->usingByObjects.erase(gameObject.GetId());
    }
}
void WaterReflectionRefractionRenderer::unSubscribeAll()
{
    skyBoxRenderer_.unSubscribeAll();
    entityRenderer_.unSubscribeAll();
    terrainMeshRenderer_.unSubscribeAll();

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribers_.clear();

    for (auto& fbo : waterFbos_)
    {
        fbo.usingByObjects.clear();
    }
}
void WaterReflectionRefractionRenderer::reloadShaders()
{
    skyBoxShader_.Reload();
    entityShader_.Reload();
    terrainShader_.Reload();
}
WaterReflectionRefractionRenderer::WaterTextures* WaterReflectionRefractionRenderer::GetWaterTextures(
    uint32 gameObjectId) const
{
    auto iter = subscribers_.find(gameObjectId);
    return iter != subscribers_.end() ? iter->second.waterTextures_ : nullptr;
}
GraphicsApi::IFrameBuffer* WaterReflectionRefractionRenderer::createWaterFbo(const vec2ui& size)
{
    GraphicsApi::FrameBuffer::Attachment depthAttachment(size, GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter   = GraphicsApi::FrameBuffer::Filter::Linear;

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
void WaterReflectionRefractionRenderer::createRefractionTexture(WaterFbo& fbo)
{
    if (not fbo.refractionFrameBuffer_)
        return;

    auto renderSize = EngineConf.renderer.water.waterRefractionResolution;
    context_.graphicsApi_.SetViewPort(0, 0, renderSize.x, renderSize.y);

    auto& camera              = context_.scene_->GetCamera();
    float waterTilePositionY  = fbo.positionY;
    auto projectionViewMatrix = context_.projection_.GetProjectionMatrix() * camera.GetViewMatrix();

    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.ProjectionViewMatrix = context_.graphicsApi_.PrepareMatrixToLoad(projectionViewMatrix);
    perFrameBuffer.cameraPosition       = camera.GetPosition();
    perFrameBuffer.clipPlane            = vec4(0.f, -1.f, 0.f, fbo.positionY);

    context_.graphicsApi_.UpdateShaderBuffer(*refractionPerFrameBuffer_, &perFrameBuffer);
    auto lastBindedShaderBuffer = context_.graphicsApi_.BindShaderBuffer(*refractionPerFrameBuffer_);

    fbo.refractionFrameBuffer_->Clear();
    fbo.refractionFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    renderScene();
    fbo.refractionFrameBuffer_->UnBind();
    context_.graphicsApi_.BindShaderBuffer(lastBindedShaderBuffer);
}
Quaternion filpPitch(const Rotation& rotation)
{
    return Quaternion(rotation.value_.w, rotation.value_.z, -rotation.value_.y, -rotation.value_.x);
}
void WaterReflectionRefractionRenderer::createReflectionTexture(WaterFbo& fbo)
{
    if (not fbo.reflectionFrameBuffer_)
        return;

    auto renderSize = EngineConf.renderer.water.waterReflectionResolution;
    context_.graphicsApi_.SetViewPort(0, 0, renderSize.x, renderSize.y);

    auto& camera = context_.scene_->GetCamera();

    auto cameraPosition = camera.GetPosition();

    float distance = 2.f * (cameraPosition.y - fbo.positionY);
    cameraPosition.y -= distance;
    glm::vec3 newforward = glm::reflect(camera.GetDirection(), VECTOR_UP);

    auto rotation             = Utils::lookAt(newforward + cameraPosition, cameraPosition);
    auto viewMatrix           = Utils::createViewMatrix(rotation, cameraPosition);
    auto projectionViewMatrix = context_.projection_.GetProjectionMatrix() * viewMatrix;

    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.ProjectionViewMatrix = context_.graphicsApi_.PrepareMatrixToLoad(projectionViewMatrix);
    perFrameBuffer.cameraPosition       = cameraPosition;
    perFrameBuffer.clipPlane            = vec4(0.f, 1.f, 0.f, -fbo.positionY);

    context_.graphicsApi_.UpdateShaderBuffer(*reflectionPerFrameBuffer_, &perFrameBuffer);
    auto lastBindedShaderBuffer = context_.graphicsApi_.BindShaderBuffer(*reflectionPerFrameBuffer_);

    fbo.reflectionFrameBuffer_->Clear();
    fbo.reflectionFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    context_.frustrum_.push(projectionViewMatrix);
    renderScene();
    context_.frustrum_.pop();
    fbo.reflectionFrameBuffer_->UnBind();
    context_.graphicsApi_.BindShaderBuffer(lastBindedShaderBuffer);
}

void WaterReflectionRefractionRenderer::cleanNotUsedFbos()
{
    for (auto iter = waterFbos_.begin(); iter != waterFbos_.end();)
    {
        if (iter->usingByObjects.empty())
        {
            if (iter->reflectionFrameBuffer_)
                context_.graphicsApi_.DeleteFrameBuffer(*iter->reflectionFrameBuffer_);
            if (iter->refractionFrameBuffer_)
                context_.graphicsApi_.DeleteFrameBuffer(*iter->refractionFrameBuffer_);

            iter = waterFbos_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

WaterReflectionRefractionRenderer::WaterFbo* WaterReflectionRefractionRenderer::getFbo(uint32 gameObjectId,
                                                                                       Subscriber& subscriber)
{
    auto currentPositionY = subscriber.gameObject.GetWorldTransform().GetPosition().y;

    WaterFbo* fbo{nullptr};
    if (subscriber.positionY)
    {
        if (compare(*subscriber.positionY, currentPositionY, POSITION_EPSILON))
        {
            fbo = findFbo(currentPositionY);
        }
        else
        {
            fbo = findFbo(*subscriber.positionY);
            if (fbo->usingByObjects.size() > 1)
            {
                fbo->usingByObjects.erase(gameObjectId);
                fbo = createWaterTilesTextures(currentPositionY);

                if (fbo)
                {
                    fbo->usingByObjects.insert(gameObjectId);
                    subscriber.positionY = currentPositionY;
                }
            }
            else
            {
                fbo->positionY       = currentPositionY;
                subscriber.positionY = currentPositionY;
            }
        }
    }
    else
    {
        fbo = findFbo(currentPositionY);
        if (fbo)
        {
            fbo->usingByObjects.insert(gameObjectId);
        }
        else
        {
            fbo = createWaterTilesTextures(currentPositionY);
            if (fbo)
            {
                fbo->usingByObjects.insert(gameObjectId);
                subscriber.positionY = currentPositionY;
            }
        }
    }
    return fbo;
}

WaterReflectionRefractionRenderer::WaterFbo* WaterReflectionRefractionRenderer::findFbo(float positionY)
{
    auto iter = std::find_if(waterFbos_.begin(), waterFbos_.end(), [positionY](const auto& fbo) {
        return compare(positionY, fbo.positionY, POSITION_EPSILON);
    });

    if (iter != waterFbos_.end())
    {
        return &(*iter);
    }

    return nullptr;
}

WaterReflectionRefractionRenderer::WaterFbo* WaterReflectionRefractionRenderer::createWaterTilesTextures(
    float positionY)
{
    DEBUG_LOG("Create new water fbo positionY=" + std::to_string(positionY));
    WaterReflectionRefractionRenderer::WaterFbo waterFbo;
    waterFbo.positionY              = positionY;
    waterFbo.reflectionFrameBuffer_ = createWaterFbo(EngineConf.renderer.water.waterReflectionResolution);
    waterFbo.refractionFrameBuffer_ = createWaterFbo(EngineConf.renderer.water.waterRefractionResolution);

    if (waterFbo.reflectionFrameBuffer_)
    {
        waterFbo.waterTextures_.waterReflectionTextureId =
            waterFbo.reflectionFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
    }

    if (waterFbo.refractionFrameBuffer_)
    {
        waterFbo.waterTextures_.waterRefractionTextureId =
            waterFbo.refractionFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
        waterFbo.waterTextures_.waterRefractionDepthTextureId =
            waterFbo.refractionFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth);
    }
    waterFbos_.push_back(waterFbo);
    return &waterFbos_.back();
}

}  // namespace GameEngine
