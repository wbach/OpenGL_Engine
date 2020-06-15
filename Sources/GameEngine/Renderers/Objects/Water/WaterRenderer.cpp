#include "WaterRenderer.h"

#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include <Logger/Log.h>

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
    : context_(context)
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
}
void WaterRenderer::prepare()
{
    // render reflect refract texture
}
void WaterRenderer::render()
{
    context_.graphicsApi_.EnableBlend();
    shader_.Start();

    WaterTileMeshBuffer waterTileMeshBuffer;
    waterTileMeshBuffer.isSimpleRender = 1.f;

    for (auto& subscriber : subscribers_)
    {
        auto& component = *subscriber.second.waterRendererComponent_;

        auto perObjectBufferId = component.getPerObjectUpdateBufferId();

        if (perObjectBufferId)
        {
            context_.graphicsApi_.BindShaderBuffer(*perObjectBufferId);
        }

        waterTileMeshBuffer.tiledValue = DEFAULT_TILED_VALUE * component.GetParentGameObject().GetWorldTransform().GetScale().x;
        waterTileMeshBuffer.moveFactor = component.increaseAndGetMoveFactor(context_.time_.deltaTime * WAVE_SPEED);
        waterTileMeshBuffer.waterColor = component.GetWaterColor();

        context_.graphicsApi_.UpdateShaderBuffer(*perMeshObjectId_, &waterTileMeshBuffer);
        context_.graphicsApi_.BindShaderBuffer(*perMeshObjectId_);

        if (component.GetNormalTexture() and component.GetNormalTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(2, *component.GetNormalTexture()->GetGraphicsObjectId());

        if (component.GetDudvTexture() and component.GetDudvTexture()->GetGraphicsObjectId())
            context_.graphicsApi_.ActiveTexture(4, *component.GetDudvTexture()->GetGraphicsObjectId());

        context_.graphicsApi_.RenderQuad();
    }
    context_.graphicsApi_.DisableBlend();
}
PerObjectUpdate WaterRenderer::CalculateTransformMatrix(const vec3& position, const vec3& scale) const
{
    return {context_.graphicsApi_.PrepareMatrixToLoad(
        Utils::CreateTransformationMatrix(position, DegreesVec3(-90, 0, 0), scale))};
}
void WaterRenderer::subscribe(GameObject& gameObject)
{
    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    if (not waterComponent)
    {
        return;
    }

    subscribers_.insert({gameObject.GetId(), {waterComponent}});
}
void WaterRenderer::unSubscribe(GameObject& gameObject)
{
    auto waterComponent = gameObject.GetComponent<Components::WaterRendererComponent>();

    if (waterComponent)
    {
        subscribers_.erase(gameObject.GetId());
    }
}
void WaterRenderer::unSubscribeAll()
{
    subscribers_.clear();
}
void WaterRenderer::reloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
