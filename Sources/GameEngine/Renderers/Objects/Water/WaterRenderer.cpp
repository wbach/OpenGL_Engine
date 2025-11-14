#include "WaterRenderer.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Objects/Water/MeshWaterFactory.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
const float WAVE_SPEED = 0.0025f;

struct WaterTileMeshBuffer
{
    AlignWrapper<vec4> waterColor;
    AlignWrapper<float> tiledValue;
    AlignWrapper<float> isSimpleRender;
    AlignWrapper<float> moveFactor;
};

const float DEFAULT_TILED_VALUE{0.01f};

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
        perMeshObjectId_ = context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(WaterTileMeshBuffer));
    }

    waterReflectionRefractionRenderer_.init();
}
void WaterRenderer::prepare()
{
    waterReflectionRefractionRenderer_.prepare();
}
void WaterRenderer::render()
{
    if (subscribers_.empty())
        return;

    context_.graphicsApi_.EnableBlend();
    context_.graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA);
    shader_.Start();

    WaterTileMeshBuffer waterTileMeshBuffer;
    waterTileMeshBuffer.isSimpleRender = 1.f;

    for (auto& subscriber : subscribers_)
    {
        auto model = subscriber.second.waterRendererComponent_.GetModel();
        if (not model)
        {
            continue;
        }
        auto& component = subscriber.second.waterRendererComponent_;

        auto isVisible =  context_.frustrum_.intersection(component.getModelBoundingBox());

        if (not isVisible)
        {
            LOG_DEBUG << "Water tile not visible. Skip rendering.";
            continue;
        }

        auto perObjectBufferId = component.getPerObjectUpdateBufferId();

        if (perObjectBufferId)
        {
            context_.graphicsApi_.BindShaderBuffer(*perObjectBufferId);
        }

        const auto waterTextures = waterReflectionRefractionRenderer_.GetWaterTextures(subscriber.first);

        if (waterTextures)
        {
            if (waterTextures->waterReflectionTextureId)
                context_.graphicsApi_.ActiveTexture(0, *waterTextures->waterReflectionTextureId);

            if (waterTextures->waterRefractionTextureId)
                context_.graphicsApi_.ActiveTexture(1, *waterTextures->waterRefractionTextureId);

            if (waterTextures->waterRefractionDepthTextureId)
                context_.graphicsApi_.ActiveTexture(2, *waterTextures->waterRefractionDepthTextureId);

            waterTileMeshBuffer.isSimpleRender = 0.f;
        }
        else
        {
            waterTileMeshBuffer.isSimpleRender = 1.f;
        }

        waterTileMeshBuffer.tiledValue = DEFAULT_TILED_VALUE * component.GetParentGameObject().GetWorldTransform().GetScale().x;
        waterTileMeshBuffer.moveFactor = component.increaseAndGetMoveFactor(context_.time_.deltaTime * WAVE_SPEED);
        waterTileMeshBuffer.waterColor = component.GetWaterColor();

        context_.graphicsApi_.UpdateShaderBuffer(*perMeshObjectId_, &waterTileMeshBuffer);
        context_.graphicsApi_.BindShaderBuffer(*perMeshObjectId_);

        if (component.GetNormalTexture() and component.GetNormalTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(3, *component.GetNormalTexture()->GetGraphicsObjectId());

        if (component.GetDudvTexture() and component.GetDudvTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(4, *component.GetDudvTexture()->GetGraphicsObjectId());

        for (const auto& mesh : model->GetMeshes())
        {
            auto isVisible =  context_.frustrum_.intersection(component.getMeshBoundingBox(mesh));
            if (isVisible and mesh.GetGraphicsObjectId())
            {
                context_.graphicsApi_.RenderTriangleStripMesh(*mesh.GetGraphicsObjectId());
            }
        }
    }
    context_.graphicsApi_.DisableBlend();
}
PerObjectUpdate WaterRenderer::CalculateTransformMatrix(const vec3& position, const vec3& scale) const
{
    return {
        context_.graphicsApi_.PrepareMatrixToLoad(Utils::CreateTransformationMatrix(position, DegreesVec3(-90, 0, 0), scale))};
}
void WaterRenderer::subscribe(GameObject& gameObject)
{
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
    waterReflectionRefractionRenderer_.unSubscribe(gameObject);

    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    if (waterComponent)
    {
        subscribers_.erase(gameObject.GetId());
    }
}
void WaterRenderer::unSubscribeAll()
{
    waterReflectionRefractionRenderer_.unSubscribeAll();
    subscribers_.clear();
}
void WaterRenderer::reloadShaders()
{
    waterReflectionRefractionRenderer_.reloadShaders();
    shader_.Reload();
}
}  // namespace GameEngine
