#include "WaterRenderer.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
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
        perMeshObjectId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(WaterTileMeshBuffer));
    }

    if (useReflectionRefractionTextures())
        waterReflectionRefractionRenderer_.init();
}
void WaterRenderer::prepare()
{
    if (useReflectionRefractionTextures())
        waterReflectionRefractionRenderer_.prepare();
}
void WaterRenderer::render()
{
    context_.graphicsApi_.EnableBlend();
    context_.graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_ALPHA);
    shader_.Start();

    WaterTileMeshBuffer waterTileMeshBuffer;
    waterTileMeshBuffer.isSimpleRender = 1.f;

    for (auto& subscriber : subscribers_)
    {
        auto& component = subscriber.second.waterRendererComponent_;

        auto perObjectBufferId = component.getPerObjectUpdateBufferId();

        if (perObjectBufferId)
        {
            context_.graphicsApi_.BindShaderBuffer(*perObjectBufferId);
        }

        waterTileMeshBuffer.tiledValue =
            DEFAULT_TILED_VALUE * component.GetParentGameObject().GetWorldTransform().GetScale().x;
        waterTileMeshBuffer.moveFactor = component.increaseAndGetMoveFactor(context_.time_.deltaTime * WAVE_SPEED);
        waterTileMeshBuffer.waterColor = component.GetWaterColor();

        context_.graphicsApi_.UpdateShaderBuffer(*perMeshObjectId_, &waterTileMeshBuffer);
        context_.graphicsApi_.BindShaderBuffer(*perMeshObjectId_);

        const auto waterTextures = waterReflectionRefractionRenderer_.GetWaterTextures(subscriber.first);

        if (waterTextures)
        {
            if (waterTextures->waterReflectionTextureId)
                context_.graphicsApi_.ActiveTexture(0, *waterTextures->waterReflectionTextureId);

            if (waterTextures->waterRefractionTextureId)
                context_.graphicsApi_.ActiveTexture(1, *waterTextures->waterRefractionTextureId);

            if (waterTextures->waterRefractionDepthTextureId)
                context_.graphicsApi_.ActiveTexture(2, *waterTextures->waterRefractionDepthTextureId);
        }

        if (component.GetNormalTexture() and component.GetNormalTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(3, *component.GetNormalTexture()->GetGraphicsObjectId());

        if (component.GetDudvTexture() and component.GetDudvTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(4, *component.GetDudvTexture()->GetGraphicsObjectId());

        context_.graphicsApi_.RenderQuad();
    }
    context_.graphicsApi_.DisableBlend();
}
bool WaterRenderer::useReflectionRefractionTextures()
{
    auto waterType = EngineConf.renderer.water.type;
    return (waterType == GameEngine::Params::WaterType::REFLECTED_REFRACTED or
            waterType == GameEngine::Params::WaterType::FULL);
}
PerObjectUpdate WaterRenderer::CalculateTransformMatrix(const vec3& position, const vec3& scale) const
{
    return {context_.graphicsApi_.PrepareMatrixToLoad(
        Utils::CreateTransformationMatrix(position, DegreesVec3(-90, 0, 0), scale))};
}
void WaterRenderer::subscribe(GameObject& gameObject)
{
    if (useReflectionRefractionTextures())
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
    if (useReflectionRefractionTextures())
        waterReflectionRefractionRenderer_.unSubscribe(gameObject);

    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    if (waterComponent)
    {
        subscribers_.erase(gameObject.GetId());
    }
}
void WaterRenderer::unSubscribeAll()
{
    if (useReflectionRefractionTextures())
        waterReflectionRefractionRenderer_.unSubscribeAll();
    subscribers_.clear();
}
void WaterRenderer::reloadShaders()
{
    if (useReflectionRefractionTextures())
        waterReflectionRefractionRenderer_.reloadShaders();
    shader_.Reload();
}
}  // namespace GameEngine
