#include "WaterRenderer.h"

#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
const float WAVE_SPEED = 0.0025f;

struct WaterTileMeshBuffer
{
    AlignWrapper<vec4> waterColor;
    AlignWrapper<float> isSimpleRender;
    AlignWrapper<float> moveFactor;
};

WaterRenderer::WaterRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Water)
{
    __RegisterRenderFunction__(RendererFunctionType::ONENDFRAME, WaterRenderer::Render);
}
WaterRenderer::~WaterRenderer()
{
    if (perObjectUpdateId_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perObjectUpdateId_);
    }

    if (perMeshObjectId_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perMeshObjectId_);
    }
}
void WaterRenderer::Init()
{
    shader_.Init();

    if (not perObjectUpdateId_)
    {
        perObjectUpdateId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    }

    if (not perMeshObjectId_)
    {
        perMeshObjectId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(WaterTileMeshBuffer));
    }
}
void WaterRenderer::Render(const Scene&, const Time& time)
{
    context_.graphicsApi_.EnableBlend();
    shader_.Start();

    WaterTileMeshBuffer waterTileMeshBuffer;
    waterTileMeshBuffer.isSimpleRender = 1.f;

    for (auto& subscriber : subscribers_)
    {
        context_.graphicsApi_.UpdateShaderBuffer(*perObjectUpdateId_, &subscriber.second.perObjectUpdate_);
        context_.graphicsApi_.BindShaderBuffer(*perObjectUpdateId_);

        auto& component = *subscriber.second.waterRendererComponent_;

        waterTileMeshBuffer.moveFactor = component.increaseAndGetMoveFactor(time.deltaTime * WAVE_SPEED);
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
void WaterRenderer::Subscribe(GameObject* gameObject)
{
    auto waterComponent = gameObject->GetComponent<Components::WaterRendererComponent>();

    if (not waterComponent)
    {
        return;
    }

    subscribers_.insert(
        {gameObject->GetId(),
         {CalculateTransformMatrix(waterComponent->GetPosition(), waterComponent->GetScale()), waterComponent}});
}
void WaterRenderer::UnSubscribe(GameObject* gameObject)
{
    auto waterComponent = gameObject->GetComponent<Components::WaterRendererComponent>();

    if (waterComponent)
    {
        subscribers_.erase(gameObject->GetId());
    }
}
void WaterRenderer::UnSubscribeAll()
{
    subscribers_.clear();
}
void WaterRenderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
