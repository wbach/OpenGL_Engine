#include "TerrainMeshRenderer.h"
#include <algorithm>
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Resources/Models/Mesh.h"

namespace GameEngine
{
TerrainMeshRenderer::TerrainMeshRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TerrainMesh)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TerrainMeshRenderer::Render);
}
TerrainMeshRenderer::~TerrainMeshRenderer()
{
}
void TerrainMeshRenderer::Init()
{
    shader_.Init();
}
void TerrainMeshRenderer::Render(const Scene&, const Time&)
{
    if (subscribes_.empty())
        return;
    context_.graphicsApi_.EnableCulling();
    shader_.Start();
    RenderSubscribers();
}
void TerrainMeshRenderer::RenderSubscribers() const
{
    for (const auto& sub : subscribes_)
    {
        RenderSubscriber(sub.second);
    }
}
void TerrainMeshRenderer::RenderSubscriber(const Subscriber& subscriber) const
{
    BindTextures(subscriber.component_->GetTextures());

    const auto& model = subscriber.component_->GetModel().Get(LevelOfDetail::L1);

    if (not model)
        return;

    int index = 0;
    for (const auto& mesh : model->GetMeshes())
    {
        context_.graphicsApi_.BindShaderBuffer(*subscriber.component_->GetPerObjectUpdateBuffer(index++));
        RenderMesh(mesh);
    }
}
void TerrainMeshRenderer::RenderModel(const Model& model) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        RenderMesh(mesh);
    }
}
void TerrainMeshRenderer::RenderMesh(const Mesh& mesh) const
{
    context_.graphicsApi_.RenderTriangleStripMesh(mesh.GetGraphicsObjectId());
}
void TerrainMeshRenderer::BindTextures(const TerrainTexturesMap& textures) const
{
    context_.graphicsApi_.ActiveTexture(0, context_.shadowsFrameBuffer_.GetShadowMap());

    for (const auto& t : textures)
        BindTexture(t.second, static_cast<int>(t.first));
}
void TerrainMeshRenderer::BindTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_.ActiveTexture(id, texture->GetGraphicsObjectId());
}
void TerrainMeshRenderer::Subscribe(GameObject* gameObject)
{
    auto terrain = gameObject->GetComponent<Components::TerrainRendererComponent>();

    if (not terrain or terrain->GetRendererType() != Components::TerrainRendererComponent::RendererType::Mesh)
        return;

    subscribes_.push_back({gameObject->GetId(), {gameObject, terrain->GetMeshTerrain()}});
}
void TerrainMeshRenderer::UnSubscribe(GameObject* gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [gameObject](const auto& obj) { return obj.first == gameObject->GetId(); });

    if (iter != subscribes_.end())
    {
        subscribes_.erase(iter);
    }
}
void TerrainMeshRenderer::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
