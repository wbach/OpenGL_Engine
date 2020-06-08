#include "RendererComponent.hpp"

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
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

RendererComponent::RendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(RendererComponent::type, componentContext, gameObject)
    , isSubscribed_(false)
    , textureIndex_(0)
{
}

RendererComponent::~RendererComponent()
{
}

void RendererComponent::CleanUp()
{
    UnSubscribe();

    for (auto model : model_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
    ClearShaderBuffers();
}

void RendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::Subscribe, this));
}
void RendererComponent::InitFromParams(const std::unordered_map<std::string, std::string>& params)
{
    UnSubscribe();

    try
    {
        if (params.count(MODEL_L1) and not params.at(MODEL_L1).empty())
            AddModel(params.at(MODEL_L1), LevelOfDetail::L1);
        if (params.count(MODEL_L2) and not params.at(MODEL_L2).empty())
            AddModel(params.at(MODEL_L2), LevelOfDetail::L2);
        if (params.count(MODEL_L3) and not params.at(MODEL_L3).empty())
            AddModel(params.at(MODEL_L3), LevelOfDetail::L3);
        if (params.count(TEXTURE_INDEX) and not params.at(TEXTURE_INDEX).empty())
            SetTextureIndex(std::stoi(params.at(TEXTURE_INDEX)));

        DEBUG_LOG("Subscribe");
        Subscribe();
    }
    catch (...)
    {
        ERROR_LOG("fail");
    }
}
std::unordered_map<ParamName, Param> RendererComponent::GetParams() const
{
    std::unordered_map<ParamName, Param> result;
    auto models = model_.Get();
    result.insert(
        {MODEL_L1,
         {MODEL_FILE,
          models.count(LevelOfDetail::L1) ? models.at(LevelOfDetail::L1)->GetFile().GetDataRelativeDir() : ""}});
    result.insert(
        {MODEL_L2,
         {MODEL_FILE,
          models.count(LevelOfDetail::L2) ? models.at(LevelOfDetail::L2)->GetFile().GetDataRelativeDir() : ""}});
    result.insert(
        {MODEL_L3,
         {MODEL_FILE,
          models.count(LevelOfDetail::L3) ? models.at(LevelOfDetail::L3)->GetFile().GetDataRelativeDir() : ""}});
    result.insert({TEXTURE_INDEX, {INT, std::to_string(textureIndex_)}});

    return result;
}
RendererComponent& RendererComponent::AddModel(const std::string& filename, GameEngine::LevelOfDetail lvl)
{
    if (filename.empty())
        return *this;

    filenames_.insert({Utils::ReplaceSlash(filename), lvl});

    auto model = componentContext_.resourceManager_.LoadModel(filename);

    if (model)
    {
        ReserveBufferVectors(model->GetMeshes().size());
        CreateBuffers(*model);

        auto existModel = model_.Get(lvl);
        if (not existModel)
        {
            model_.Add(model, lvl);
        }
        else
        {
            model_.Update(model, lvl);
            componentContext_.resourceManager_.ReleaseModel(*existModel);
        }
    }

    return *this;
}
RendererComponent& RendererComponent::SetTextureIndex(uint32_t index)
{
    textureIndex_ = index;
    return *this;
}
void RendererComponent::ClearShaderBuffers()
{
    for (auto iter = perObjectUpdateBuffer_.begin(); iter != perObjectUpdateBuffer_.end();)
    {
        DeleteShaderBuffer(std::move(*iter));
        iter = perObjectUpdateBuffer_.erase(iter);
    }

    for (auto iter = perObjectConstantsBuffer_.begin(); iter != perObjectConstantsBuffer_.end();)
    {
        DeleteShaderBuffer(std::move(*iter));
        iter = perObjectConstantsBuffer_.erase(iter);
    }
}
void RendererComponent::DeleteShaderBuffer(std::unique_ptr<GpuObject> obj)
{
    componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(obj));
}
void RendererComponent::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void RendererComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
void RendererComponent::ReserveBufferVectors(size_t size)
{
    ClearShaderBuffers();
    perObjectUpdateBuffer_.reserve(size);
    perObjectConstantsBuffer_.reserve(size);
}
void RendererComponent::CreateBuffers(Model& model)
{
    for (auto& mesh : model.GetMeshes())
    {
        CreatePerObjectUpdateBuffer(mesh);
        CreatePerObjectConstantsBuffer(mesh);
    }
}
void RendererComponent::CreatePerObjectUpdateBuffer(const Mesh& mesh)
{
    auto& graphicsApi = componentContext_.graphicsApi_;

    perObjectUpdateBuffer_.push_back(
        std::make_unique<BufferObject<PerObjectUpdate>>(graphicsApi, PER_OBJECT_UPDATE_BIND_LOCATION));

    auto& buffer = *perObjectUpdateBuffer_.back();

    const mat4 transformMatrix            = thisObject_.GetWorldTransform().GetMatrix() * mesh.GetMeshTransform();
    buffer.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(transformMatrix);
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(buffer);
}
void RendererComponent::CreatePerObjectConstantsBuffer(const Mesh& mesh)
{
    perObjectConstantsBuffer_.push_back(std::make_unique<BufferObject<PerObjectConstants>>(
        componentContext_.graphicsApi_, PER_OBJECT_CONSTANTS_BIND_LOCATION));

    auto& buffer = *perObjectConstantsBuffer_.back();

    if (mesh.GetMaterial().diffuseTexture)
    {
        buffer.GetData().textureOffset = mesh.GetMaterial().diffuseTexture->GetTextureOffset(textureIndex_);
    }
    else
    {
        buffer.GetData().textureOffset = vec2(0);
    }
    buffer.GetData().UseBoneTransform = mesh.UseArmature();

    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(buffer);
}
void RendererComponent::UpdateBuffers()
{
    size_t index = 0;
    thisObject_.TakeWorldTransfromSnapshot();

    auto model = model_.Get(LevelOfDetail::L1);

    if (model)
    {
        for (auto& mesh : model_.Get(LevelOfDetail::L1)->GetMeshes())
        {
            auto& poc = perObjectUpdateBuffer_[index++];

            if (poc)
            {
                const mat4 transformMatix = thisObject_.GetWorldTransform().GetMatrix() * mesh.GetMeshTransform();
                poc->GetData().TransformationMatrix =
                    componentContext_.graphicsApi_.PrepareMatrixToLoad(transformMatix);
                poc->UpdateGpuPass();
            }
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
