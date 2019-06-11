#include "WaterRenderer.h"
#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Shaders/IShaderFactory.h"

namespace GameEngine
{
const float WAVE_SPEED = 0.0025f;

struct WaterTileMeshBuffer
{
    AlignWrapper<float> isSimpleRender;
    AlignWrapper<float> moveFactor;
    vec4 waterColor;
};

WaterRenderer::WaterRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Water);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, WaterRenderer::Render);
}
void WaterRenderer::Init()
{
    shader_->Init();

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
    shader_->Start();

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

        if (component.GetDudvTexture())
            context_.graphicsApi_.ActiveTexture(4, component.GetDudvTexture()->GetId());

        if (component.GetNormalTexture())
            context_.graphicsApi_.ActiveTexture(2, component.GetNormalTexture()->GetId());

        context_.graphicsApi_.RenderQuad();
    }
}
PerObjectUpdate WaterRenderer::CalculateTransformMatrix(const vec3& position, const vec3& scale) const
{
    return {Utils::CreateTransformationMatrix(position, vec3(90, 0, 0), scale)};
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
    subscribers_.erase(gameObject->GetId());
}
void WaterRenderer::UnSubscribeAll()
{
    subscribers_.clear();
}
void WaterRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}
}  // namespace GameEngine
