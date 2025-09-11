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
constexpr char COMPONENT_STR[]         = "Renderer";
constexpr char CSTR_TEXTURE_INDEX[]    = "textureIndex";
constexpr char CSTR_MODEL_FILE_NAMES[] = "modelFileNames";
constexpr char MODEL_NORMALIZATION[]   = "modelNormalization";
constexpr char MESH_OPTIMIZE[]         = "meshOptimize";
const GraphicsApi::ID defaultId;
}  // namespace

RendererComponent::RendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(COMPONENT_STR, componentContext, gameObject)
    , textureIndex(0)
    , isSubscribed_(false)
    , loadingParameters_{DEFAULT_LOADING_PARAMETERS}
{
    modelNormalization = loadingParameters_.modelNormalization == ModelNormalization::normalized ? true : false;
    meshOptimize       = loadingParameters_.meshOptimize == MeshOptimize::optimized ? true : false;
}

void RendererComponent::CleanUp()
{
    UnSubscribe();

    for (auto model : model_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
    ClearShaderBuffers();

    if (worldTransformSub_)
        thisObject_.UnsubscribeOnWorldTransfromChange(*worldTransformSub_);
}

void RendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::init, this));
}

RendererComponent& RendererComponent::AddModel(Model* model, LevelOfDetail i)
{
    model_.Add(model, i);
    return *this;
}

RendererComponent& RendererComponent::AddModel(const std::string& filename, GameEngine::LevelOfDetail lvl)
{
    if (filename.empty())
        return *this;

    switch (lvl)
    {
        case GameEngine::LevelOfDetail::L1:
            fileName_LOD1 = Utils::ReplaceSlash(filename);
            break;
        case GameEngine::LevelOfDetail::L2:
            fileName_LOD2 = Utils::ReplaceSlash(filename);
            break;
        case GameEngine::LevelOfDetail::L3:
            fileName_LOD3 = Utils::ReplaceSlash(filename);
            break;
    }
    return *this;
}
RendererComponent& RendererComponent::SetTextureIndex(uint32_t index)
{
    textureIndex = index;
    return *this;
}
void RendererComponent::init()
{
    loadingParameters_.modelNormalization = modelNormalization ? ModelNormalization::normalized : ModelNormalization::none;
    loadingParameters_.meshOptimize       = meshOptimize ? MeshOptimize::optimized : MeshOptimize::none;

    bool atLeastOneModelIsCreated{false};
    for (auto& [lvl, file] : GetFiles())
    {
        if (file.empty())
            continue;

        DEBUG_LOG(thisObject_.GetName() + " Load model: " + file.GetBaseName());
        auto model = componentContext_.resourceManager_.LoadModel(file, loadingParameters_);

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

    //    worldTransformSub_ =
    //        thisObject_.SubscribeOnWorldTransfomChange([this](const common::Transform&) { UpdateBuffers(); }); // TO
    //        DO
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

    auto bufferPtr =
        std::make_unique<BufferObject<PerObjectConstants>>(componentContext_.graphicsApi_, PER_OBJECT_CONSTANTS_BIND_LOCATION);
    auto& buffer = *bufferPtr.get();

    perObjectConstantsBuffer_.insert({mesh.GetGpuObjectId(), std::move(bufferPtr)});

    if (mesh.GetMaterial().diffuseTexture)
    {
        buffer.GetData().textureOffset = mesh.GetMaterial().diffuseTexture->GetTextureOffset(textureIndex);
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
                auto& buffer                          = *iter->second;
                const mat4 transformMatix             = thisObject_.GetWorldTransform().GetMatrix() * mesh.GetMeshTransform();
                buffer.GetData().TransformationMatrix = componentContext_.graphicsApi_.PrepareMatrixToLoad(transformMatix);
                buffer.UpdateGpuPass();
                // componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(buffer);
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

std::unordered_map<LevelOfDetail, File> RendererComponent::GetFiles() const
{
    return {{LevelOfDetail::L1, fileName_LOD1}, {LevelOfDetail::L2, fileName_LOD2}, {LevelOfDetail::L3, fileName_LOD3}};
}

void RendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<RendererComponent>(componentContext, gameObject);

        auto textureIndexNode = node.getChild(CSTR_TEXTURE_INDEX);
        if (textureIndexNode)
        {
            try
            {
                component->textureIndex = std::stoul(textureIndexNode->value_);
            }
            catch (...)
            {
                ERROR_LOG("SetTextureIndex index error");
            }
        }

        auto modelNormalization = node.getChild(MODEL_NORMALIZATION);
        if (modelNormalization)
        {
            component->modelNormalization = Utils::StringToBool(modelNormalization->value_);
        }
        auto meshOptimize = node.getChild(MESH_OPTIMIZE);
        if (meshOptimize)
        {
            component->meshOptimize = Utils::StringToBool(meshOptimize->value_);
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
                        auto lodInt          = std::stoi(lodNode->value_);
                        if (lodInt == 0)
                        {
                            DEBUG_LOG(filename);
                            component->fileName_LOD1 = filename;
                        }
                        else if (lodInt == 1)
                        {
                            DEBUG_LOG(filename);
                            component->fileName_LOD2 = filename;
                        }
                        else if (lodInt == 2)
                        {
                            DEBUG_LOG(filename);
                            component->fileName_LOD3 = filename;
                        }
                        else
                        {
                            DEBUG_LOG("LOD \"" + std::to_string(lodInt) + "\" is out of range. Correct range is 0-2");
                        }
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
void create(TreeNode& node, const std::filesystem::path& file, LevelOfDetail lvl)
{
    node.addChild(CSTR_FILE_NAME, file);
    node.addChild(CSTR_MODEL_LVL_OF_DETAIL, std::to_string(static_cast<int>(lvl)));
}
void create(TreeNode& node, const std::unordered_map<LevelOfDetail, File>& files)
{
    for (const auto& [lodLvl, file] : files)
    {
        create(node.addChild(CSTR_MODEL_FILE_NAME), file.GetDataRelativePath(), lodLvl);
    }
}
void RendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    node.addChild(CSTR_TEXTURE_INDEX, std::to_string(textureIndex));
    node.addChild(MODEL_NORMALIZATION, Utils::BoolToString(modelNormalization));
    node.addChild(MESH_OPTIMIZE, Utils::BoolToString(meshOptimize));

    create(node.addChild(CSTR_MODEL_FILE_NAMES), GetFiles());
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
