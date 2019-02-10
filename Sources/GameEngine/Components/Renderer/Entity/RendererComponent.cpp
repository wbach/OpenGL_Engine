#include "RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
ComponentsType RendererComponent::type = ComponentsType::Renderer;

RendererComponent::RendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Renderer, componentContext, gameObject)
    , textureIndex_(0)
{
}

RendererComponent::~RendererComponent()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}

void RendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::Subscribe, this));
}
RendererComponent& RendererComponent::AddModel(const std::string& filename, GameEngine::LevelOfDetail lvl)
{
    if (filename.empty())
        return *this;

    filenames_.insert({filename, lvl});

    ModelRawPtr model = componentContext_.resourceManager_.LoadModel(filename);

    thisObject_.worldTransform.TakeSnapShoot();

    ReserveBufferVectors(model->GetMeshes().size());
    CreateBuffers(model);

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(model);
    model_.Add(model, lvl);

    return *this;
}
RendererComponent& RendererComponent::SetModel(const ModelWrapper& model)
{
    model_ = model;
    return *this;
}
RendererComponent& RendererComponent::SetTextureIndex(uint32_t index)
{
    textureIndex_ = index;
    return *this;
}
void RendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void RendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
void RendererComponent::ReserveBufferVectors(uint32 size)
{
    perObjectUpdate_.reserve(size);
    perObjectUpdateBuffer_.reserve(size);
    perObjectConstants_.reserve(size);
    perObjectConstantsBuffer_.reserve(size);
}
void RendererComponent::CreateBuffers(ModelRawPtr model)
{
    for (auto& mesh : model->GetMeshes())
    {
        CreatePerObjectUpdateBuffer(mesh);
        CreatePerObjectConstantsBuffer(mesh);
    }
}
void RendererComponent::CreatePerObjectUpdateBuffer(const Mesh& mesh)
{
    perObjectUpdate_.emplace_back();
    auto& pu                = perObjectUpdate_.back();
    pu.TransformationMatrix = thisObject_.worldTransform.GetMatrix() * mesh.GetMeshTransform();

    BufferObject obj(componentContext_.resourceManager_.GetGraphicsApi(), PER_OBJECT_UPDATE_BIND_LOCATION, &pu,
                     sizeof(PerObjectUpdate));
    perObjectUpdateBuffer_.push_back(obj);

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(&perObjectUpdateBuffer_.back());
}
void RendererComponent::CreatePerObjectConstantsBuffer(const Mesh& mesh)
{
    perObjectConstants_.emplace_back();
    auto& poc            = perObjectConstants_.back();
    poc.UseBoneTransform = mesh.UseArmature();

    if (mesh.GetMaterial().diffuseTexture)
    {
        poc.textureOffset = mesh.GetMaterial().diffuseTexture->GetTextureOffset(textureIndex_);
    }
    else
    {
        poc.textureOffset = vec2(0);
    }

    BufferObject obj(componentContext_.resourceManager_.GetGraphicsApi(), PER_OBJECT_CONSTANTS_BIND_LOCATION, &poc,
                     sizeof(PerObjectConstants));
    perObjectConstantsBuffer_.push_back(obj);

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(
        &perObjectConstantsBuffer_.back());
}
void RendererComponent::UpdateBuffers()
{
    int index = 0;

    for (auto& mesh : model_.Get(LevelOfDetail::L1)->GetMeshes())
    {
        auto& poc = perObjectUpdate_[index];
        auto& pocb = perObjectUpdateBuffer_[index];

        poc.TransformationMatrix = thisObject_.worldTransform.GetMatrix() * mesh.GetMeshTransform();
        pocb.UpdateBuffer();
        ++index;
    }
}
}  // namespace Components
}  // namespace GameEngine