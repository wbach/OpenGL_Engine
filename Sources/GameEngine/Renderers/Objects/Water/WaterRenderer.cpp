#include "WaterRenderer.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include <mutex>

#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Objects/Water/MeshWaterFactory.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Types.h"

namespace GameEngine
{
struct WaterTileMeshBuffer
{
    AlignWrapper<vec4> waterColor;
    AlignWrapper<vec4> tilePosAndScale;
    AlignWrapper<vec4> params;  // x - planeMoveFactor, y - waveFactor, z - tiledValue, w - isSimpleRender
    AlignWrapper<vec4> waveParams;
    AlignWrapper<vec4> projParams;
    AlignWrapper<vec4> waterDepthVisibility;  // x - maxVisibleDepth, y = scaleDepth
};

const float useSimpleRender{1.f};
const float useAdvancedRender{0.f};

WaterRenderer::WaterRenderer(RendererContext& context)
    : waterReflectionRefractionRenderer_(context)
    , context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Water)
{
}
WaterRenderer::~WaterRenderer()
{
    if (perMeshObjectId_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perMeshObjectId_);
    }
}
void WaterRenderer::init()
{
    shader_.Init();

    if (not perMeshObjectId_)
    {
        perMeshObjectId_ = context_.graphicsApi_.CreateShaderBuffer(WATER_TILE_BIND_LOCATION, sizeof(WaterTileMeshBuffer));
    }

    waterReflectionRefractionRenderer_.init();
}
void WaterRenderer::prepare()
{
    waterReflectionRefractionRenderer_.prepare();
}
void WaterRenderer::render()
{
    std::lock_guard<std::mutex> lk(subscribersMutex_);
    if (subscribers_.empty())
        return;

    context_.graphicsApi_.EnableBlend();
    context_.graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA);
    shader_.Start();

    WaterTileMeshBuffer waterTileMeshBuffer{};
    waterTileMeshBuffer.waterColor         = vec4(0, 0, 0, 1.f);
    waterTileMeshBuffer.params             = vec4(0, 0, 0, 0.f);
    waterTileMeshBuffer.params.value.w     = useSimpleRender;
    waterTileMeshBuffer.projParams.value.x = context_.projection_.GetNear();
    waterTileMeshBuffer.projParams.value.y = context_.projection_.GetFar();

    for (auto& [gameObjectId, subscriber] : subscribers_)
    {
        auto model = subscriber.waterRendererComponent_.GetModel();
        if (not model)
        {
            continue;
        }
        auto& component = subscriber.waterRendererComponent_;

        auto isVisible = context_.frustrum_.intersection(component.getModelBoundingBox());

        if (not isVisible)
        {
            continue;
        }

        auto perObjectBufferId = component.getPerObjectUpdateBufferId();

        if (perObjectBufferId)
        {
            context_.graphicsApi_.BindShaderBuffer(*perObjectBufferId);
        }

        const auto waterTextures = waterReflectionRefractionRenderer_.GetWaterTextures(gameObjectId);

        if (waterTextures)
        {
            if (waterTextures->waterReflectionTextureId)
                context_.graphicsApi_.ActiveTexture(0, *waterTextures->waterReflectionTextureId);

            if (waterTextures->waterRefractionTextureId)
                context_.graphicsApi_.ActiveTexture(1, *waterTextures->waterRefractionTextureId);

            if (waterTextures->waterRefractionDepthTextureId)
                context_.graphicsApi_.ActiveTexture(2, *waterTextures->waterRefractionDepthTextureId);

            waterTileMeshBuffer.params.value.w = useAdvancedRender;
        }
        else
        {
            waterTileMeshBuffer.params.value.w = useSimpleRender;
        }

        component.increaseFactors(context_.time_.deltaTime);

        waterTileMeshBuffer.waterColor                   = component.GetWaterColor();
        waterTileMeshBuffer.params.value.x               = component.moveFactor();
        waterTileMeshBuffer.params.value.y               = component.waveMoveFactor();
        waterTileMeshBuffer.params.value.z               = component.tiledValue;
        waterTileMeshBuffer.waterDepthVisibility.value.x = component.maxDepthVisibility;
        waterTileMeshBuffer.waterDepthVisibility.value.y = component.depthBlendScale;

        const auto& worldTransfrom                  = component.GetParentGameObject().GetWorldTransform();
        const auto& scale                           = worldTransfrom.GetScale();
        auto position                               = worldTransfrom.GetPosition();
        waterTileMeshBuffer.waveParams.value.x      = component.waveAmplitude;
        waterTileMeshBuffer.waveParams.value.y      = component.waveFrequency;
        waterTileMeshBuffer.waveParams.value.z      = component.meshResolution;
        waterTileMeshBuffer.tilePosAndScale.value.x = position.x;
        waterTileMeshBuffer.tilePosAndScale.value.y = position.y;
        waterTileMeshBuffer.tilePosAndScale.value.z = scale.x;
        waterTileMeshBuffer.tilePosAndScale.value.w = scale.z;

        context_.graphicsApi_.UpdateShaderBuffer(*perMeshObjectId_, &waterTileMeshBuffer);
        context_.graphicsApi_.BindShaderBuffer(*perMeshObjectId_);

        if (component.GetNormalTexture() and component.GetNormalTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(3, *component.GetNormalTexture()->GetGraphicsObjectId());

        if (component.GetDudvTexture() and component.GetDudvTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(4, *component.GetDudvTexture()->GetGraphicsObjectId());

        for (const auto& mesh : model->GetMeshes())
        {
            auto isVisible = context_.frustrum_.intersection(component.getMeshBoundingBox(mesh));
            if (isVisible and mesh.GetGraphicsObjectId())
            {
                context_.graphicsApi_.RenderTriangleStripMesh(*mesh.GetGraphicsObjectId());
            }
        }
    }
    context_.graphicsApi_.DisableBlend();
}
void WaterRenderer::subscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscribersMutex_);
    if (subscribers_.find(gameObject.GetId()) != subscribers_.end())
        return;

    waterReflectionRefractionRenderer_.subscribe(gameObject);

    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    if (not waterComponent)
    {
        return;
    }

    subscribers_.insert({gameObject.GetId(), {*waterComponent}});
}
void WaterRenderer::unSubscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscribersMutex_);
    waterReflectionRefractionRenderer_.unSubscribe(gameObject);

    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    if (waterComponent)
    {
        subscribers_.erase(gameObject.GetId());
    }
}
void WaterRenderer::unSubscribeAll()
{
    std::lock_guard<std::mutex> lk(subscribersMutex_);
    waterReflectionRefractionRenderer_.unSubscribeAll();
    subscribers_.clear();
}
void WaterRenderer::reloadShaders()
{
    waterReflectionRefractionRenderer_.reloadShaders();
    shader_.Reload();
}
}  // namespace GameEngine
