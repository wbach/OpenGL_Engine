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
namespace
{
const std::string MODEL_L1      = "model_l1";
const std::string MODEL_L2      = "model_l2";
const std::string MODEL_L3      = "model_l3";
const std::string TEXTURE_INDEX = "textureIndex";
}  // namespace
ComponentsType RendererComponent::type = ComponentsType::Renderer;

RendererComponent::RendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(RendererComponent::type, componentContext, gameObject)
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
void RendererComponent::InitFromParams(std::unordered_map<std::string, std::string> params)
{
    if (params.count(MODEL_L1))
        AddModel(params.at(MODEL_L1), LevelOfDetail::L1);
    if (params.count(MODEL_L2))
        AddModel(params.at(MODEL_L2), LevelOfDetail::L2);
    if (params.count(MODEL_L3))
        AddModel(params.at(MODEL_L3), LevelOfDetail::L3);
    if (params.count(TEXTURE_INDEX))
        SetTextureIndex(std::stoi(params.at(TEXTURE_INDEX)));
}
std::unordered_map<ParamName, Param> RendererComponent::GetParams() const
{
    std::unordered_map<ParamName, Param> result;

    for (const auto& p : model_.Get())
    {
        if (p.first == LevelOfDetail::L1)
        {
            result.insert({MODEL_L1, {STRING, p.second->GetFileName()}});
        }
        else if (p.first == LevelOfDetail::L2)
        {
            result.insert({MODEL_L2, {STRING, p.second->GetFileName()}});
        }
        else if (p.first == LevelOfDetail::L3)
        {
            result.insert({MODEL_L3, {STRING, p.second->GetFileName()}});
        }
    }

    result.insert({TEXTURE_INDEX, {INT, std::to_string(textureIndex_)}});
    return result;
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
void RendererComponent::ReserveBufferVectors(size_t size)
{
    perObjectUpdateBuffer_.reserve(size);
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
    BufferObject<PerObjectUpdate> buffer(componentContext_.resourceManager_.GetGraphicsApi(),
                                         PER_OBJECT_UPDATE_BIND_LOCATION);

    auto& graphicsApi = componentContext_.resourceManager_.GetGraphicsApi();

    const mat4 viewProjectionMatrix = thisObject_.worldTransform.GetMatrix() * mesh.GetMeshTransform();

    buffer.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(viewProjectionMatrix);
    perObjectUpdateBuffer_.push_back(buffer);

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(&perObjectUpdateBuffer_.back());
}
void RendererComponent::CreatePerObjectConstantsBuffer(const Mesh& mesh)
{
    BufferObject<PerObjectConstants> buffer(componentContext_.resourceManager_.GetGraphicsApi(),
                                            PER_OBJECT_CONSTANTS_BIND_LOCATION);

    buffer.GetData().UseBoneTransform = mesh.UseArmature();

    if (mesh.GetMaterial().diffuseTexture)
    {
        buffer.GetData().textureOffset = mesh.GetMaterial().diffuseTexture->GetTextureOffset(textureIndex_);
    }
    else
    {
        buffer.GetData().textureOffset = vec2(0);
    }

    perObjectConstantsBuffer_.push_back(buffer);

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(
        &perObjectConstantsBuffer_.back());
}
void RendererComponent::UpdateBuffers()
{
    int index = 0;

    for (auto& mesh : model_.Get(LevelOfDetail::L1)->GetMeshes())
    {
        auto& poc = perObjectUpdateBuffer_[index++];

        auto& graphicsApi = componentContext_.resourceManager_.GetGraphicsApi();
        const mat4 vpm    = thisObject_.worldTransform.GetMatrix() * mesh.GetMeshTransform();

        poc.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(vpm);
        poc.UpdateBuffer();
    }
}
}  // namespace Components
}  // namespace GameEngine
