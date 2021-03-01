#include "RendererComponent.hpp"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
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
const std::string COMPONENT_STR{"Renderer"};
const std::string CSTR_TEXTURE_INDEX    = "textureIndex";
const std::string CSTR_MODEL_FILE_NAMES = "modelFileNames";
const std::string MODEL_L1              = "model_l1";
const std::string MODEL_L2              = "model_l2";
const std::string MODEL_L3              = "model_l3";
const std::string TEXTURE_INDEX         = "textureIndex";
const GraphicsApi::ID defaultId;
}  // namespace
RendererComponent::RendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(RendererComponent).hash_code(), componentContext, gameObject)
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
    RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::init, this));
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
    auto models = model_.GetAll();
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
    return *this;
}
RendererComponent& RendererComponent::SetTextureIndex(uint32_t index)
{
    textureIndex_ = index;
    return *this;
}
void RendererComponent::init()
{
    bool atLeastOneModelIsCreated{false};
    for (auto& [filename, lvl] : filenames_)
    {
        DEBUG_LOG(filename);
        auto model = componentContext_.resourceManager_.LoadModel(filename);

        if (model)
        {
            // ReserveBufferVectors(model->GetMeshes().size());
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

            atLeastOneModelIsCreated = true;
        }
        else
        {
            ERROR_LOG("nullptr");
        }
    }
    if (atLeastOneModelIsCreated)
        Subscribe();
}
void RendererComponent::ClearShaderBuffers()
{
    for (auto iter = perObjectUpdateBuffer_.begin(); iter != perObjectUpdateBuffer_.end();)
    {
        DeleteShaderBuffer(std::move(iter->second));
        iter = perObjectUpdateBuffer_.erase(iter);
    }

    for (auto iter = perObjectConstantsBuffer_.begin(); iter != perObjectConstantsBuffer_.end();)
    {
        DeleteShaderBuffer(std::move(iter->second));
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
    auto iter = perObjectUpdateBuffer_.find(mesh.GetGpuObjectId());
    if (iter != perObjectUpdateBuffer_.end())
    {
        ERROR_LOG("perObjectUpdateBuffer object already exist!");
        return;
    }

    auto& graphicsApi = componentContext_.graphicsApi_;

    auto bufferPtr = std::make_unique<BufferObject<PerObjectUpdate>>(graphicsApi, PER_OBJECT_UPDATE_BIND_LOCATION);
    auto& buffer   = *bufferPtr.get();
    perObjectUpdateBuffer_.insert({mesh.GetGpuObjectId(), std::move(bufferPtr)});

    const mat4 transformMatrix            = thisObject_.GetWorldTransform().CalculateCurrentMatrix() * mesh.GetMeshTransform();
    buffer.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(transformMatrix);
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(buffer);
}
void RendererComponent::CreatePerObjectConstantsBuffer(const Mesh& mesh)
{
    auto iter = perObjectConstantsBuffer_.find(mesh.GetGpuObjectId());
    if (iter != perObjectConstantsBuffer_.end())
    {
        ERROR_LOG("perObjectConstantsBuffer object already exist!");
        return;
    }

    auto bufferPtr = std::make_unique<BufferObject<PerObjectConstants>>(componentContext_.graphicsApi_,
                                                                        PER_OBJECT_CONSTANTS_BIND_LOCATION);
    auto& buffer   = *bufferPtr.get();

    perObjectConstantsBuffer_.insert({mesh.GetGpuObjectId(), std::move(bufferPtr)});

    if (mesh.GetMaterial().diffuseTexture)
    {
        buffer.GetData().textureOffset = mesh.GetMaterial().diffuseTexture->GetTextureOffset(textureIndex_);
    }
    else
    {
        buffer.GetData().textureOffset = vec2(0);
    }

    buffer.GetData().UseBoneTransform = 0.f;  // mesh.UseArmature();

    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(buffer);
}
void RendererComponent::UpdateBuffers()
{
    thisObject_.TakeWorldTransfromSnapshot();

    for (const auto& [_, model] : model_.GetAll())
    {
        for (const auto& mesh : model->GetMeshes())
        {
            auto iter = perObjectUpdateBuffer_.find(mesh.GetGpuObjectId());
            if (iter != perObjectUpdateBuffer_.end() and iter->second)
            {
                auto& buffer              = *iter->second;
                const mat4 transformMatix = thisObject_.GetWorldTransform().GetMatrix() * mesh.GetMeshTransform();
                buffer.GetData().TransformationMatrix =
                    componentContext_.graphicsApi_.PrepareMatrixToLoad(transformMatix);
                buffer.UpdateGpuPass();
            }
            else
            {
                ERROR_LOG("perObjectUpdateBuffer not found! : " + thisObject_.GetName() +
                          ", id : " + std::to_string(thisObject_.GetId()));
            }
        }
    }
}
void RendererComponent::useArmature(bool value)
{
    for (const auto& [_, model] : model_.GetAll())
    {
        if (not model)
            continue;

        for (auto& mesh : model->GetMeshes())
        {
            auto iter = perObjectConstantsBuffer_.find(mesh.GetGpuObjectId());
            if (iter != perObjectConstantsBuffer_.end() and iter->second)
            {
                auto& buffer           = *iter->second;
                float useBoneTransform = (value and mesh.UseArmature()) ? 1.f : 0.f;
                if (not compare(buffer.GetData().UseBoneTransform.value, useBoneTransform, 0.1f))
                {
                    buffer.GetData().UseBoneTransform = useBoneTransform;
                    componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(buffer);
                }
            }
            else
            {
                ERROR_LOG("perObjectUpdateBuffer not found! : " + thisObject_.GetName() +
                          ", id : " + std::to_string(thisObject_.GetId()));
            }
        }
    }
}
void create(TreeNode& node, const std::string& filename, LevelOfDetail lvl)
{
    node.addChild(CSTR_FILE_NAME, filename);
    node.addChild(CSTR_MODEL_LVL_OF_DETAIL, std::to_string(static_cast<int>(lvl)));
}
void create(TreeNode& node, const std::unordered_map<std::string, LevelOfDetail>& files)
{
    for (const auto& files : files)
    {
        create(node.addChild(CSTR_MODEL_FILE_NAME), files.first, files.second);
    }
}
void RendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<RendererComponent>(componentContext, gameObject);

        auto textureIndexNode = node.getChild(CSTR_TEXTURE_INDEX);
        if (textureIndexNode)
        {
            try
            {
                auto textureIndex = std::stoul(textureIndexNode->value_);
                component->SetTextureIndex(textureIndex);
            }
            catch (...)
            {
                ERROR_LOG("SetTextureIndex index error");
            }
        }

        auto modelFileNamesNode = node.getChild(CSTR_MODEL_FILE_NAMES);
        if (modelFileNamesNode)
        {
            for (const auto& fileNode : modelFileNamesNode->getChildren())
            {
                auto filenameNode = fileNode->getChild(CSTR_FILE_NAME);
                auto lodNode      = fileNode->getChild(CSTR_MODEL_LVL_OF_DETAIL);
                if (filenameNode and lodNode)
                {
                    try
                    {
                        const auto& filename = filenameNode->value_;
                        auto lod             = static_cast<LevelOfDetail>(std::stoi(lodNode->value_));
                        component->AddModel(filename, lod);
                    }
                    catch (...)
                    {
                        ERROR_LOG("Set model filenames error");
                    }
                }
            }
        }

        return component;
    };

    ReadFunctions::instance().componentsReadFunctions.insert({COMPONENT_STR, readFunc});
}
void RendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_TEXTURE_INDEX, std::to_string(textureIndex_));
    create(node.addChild(CSTR_MODEL_FILE_NAMES), filenames_);
}
const GraphicsApi::ID& RendererComponent::GetPerObjectUpdateBuffer(uint64 meshId) const
{
    auto iter = perObjectUpdateBuffer_.find(meshId);
    if (iter != perObjectUpdateBuffer_.end())
    {
        return iter->second ? iter->second->GetGraphicsObjectId() : defaultId;
    }

    return defaultId;
}
const GraphicsApi::ID& RendererComponent::GetPerObjectConstantsBuffer(uint64 meshId) const
{
    auto iter = perObjectConstantsBuffer_.find(meshId);
    if (iter != perObjectConstantsBuffer_.end())
    {
        return iter->second ? iter->second->GetGraphicsObjectId() : defaultId;
    }

    return defaultId;
}
}  // namespace Components
}  // namespace GameEngine
