#include "RendererComponent.hpp"

#include <memory>
#include <optional>
#include <vector>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Models/Primitive.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Logger/Log.h"
#include "TreeNode.h"
#include "Utils.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_TEXTURE_INDEX[]    = "textureIndex";
constexpr char CSTR_MODEL_FILE_NAMES[] = "modelFileNames";
constexpr char MODEL_NORMALIZATION[]   = "modelNormalization";
constexpr char MESH_OPTIMIZE[]         = "meshOptimize";
constexpr char MATERIALS[]             = "materials";
constexpr char CSTR_PRIMITIVES[]       = "primitives";
constexpr char CSTR_PRIMITIVE[]        = "primitive";
const GraphicsApi::ID defaultId;

}  // namespace

CustomMaterialData::CustomMaterialData(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& loader,
                                       const Material& material)
    : material{material}
    , loader{loader}
{
    CreateBufferObject(graphicsApi);
}

CustomMaterialData::~CustomMaterialData()
{
    loader.AddObjectToRelease(std::move(perMeshBuffer));
}

void CustomMaterialData::CreateBufferObject(GraphicsApi::IGraphicsApi& graphicsApi)
{
    perMeshBuffer =
        std::make_unique<BufferObject<PerMeshObject>>(createPerMeshObject(material), graphicsApi, PER_MESH_OBJECT_BIND_LOCATION);
    LOG_DEBUG << "Create custom material " << material.name;
    loader.AddObjectToGpuLoadingPass(*perMeshBuffer);
}

GraphicsApi::ID CustomMaterialData::GetBufferId() const
{
    return perMeshBuffer->GetGraphicsObjectId();
}

RendererComponent::RendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<RendererComponent>(), componentContext, gameObject)
    , textureIndex(0)
    , isSubscribed_(false)
    , loadingParameters_{DEFAULT_LOADING_PARAMETERS}
{
    modelNormalization = loadingParameters_.modelNormalization == ModelNormalization::normalized ? true : false;
    meshOptimize       = loadingParameters_.meshOptimize == MeshOptimize::optimized ? true : false;
}

void RendererComponent::CleanUp()
{
    CleanUpWithRestrictions();
}

void RendererComponent::CleanUpWithRestrictions(const std::set<Model*>& restrictedModels)
{
    UnSubscribe();

    customMaterials.clear();

    std::vector<LevelOfDetail> toRelease;
    for (auto [lod, model] : model_.GetAll())
    {
        if (not restrictedModels.contains(model))
        {
            componentContext_.resourceManager_.ReleaseModel(*model);
            toRelease.push_back(lod);
        }
        else
        {
            LOG_DEBUG << "skip added manually model : " << model->GetFile().GetAbsolutePath();
        }
    }

    for (auto lod : toRelease)
    {
        model_.clear(lod);
    }

    ClearShaderBuffers();

    if (worldTransformSub_)
    {
        thisObject_.UnsubscribeOnWorldTransfromChange(*worldTransformSub_);
        worldTransformSub_ = std::nullopt;
    }
}

void RendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::init, this));
}

void RendererComponent::Reload()
{
    CleanUpWithRestrictions(addedModels);
    init();
}

RendererComponent& RendererComponent::AddModel(Model* model, LevelOfDetail i)
{
    model_.Add(model, i);
    addedModels.insert(model);
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

    for (auto& [lvl, file] : GetFiles())
    {
        if (file.empty())
            continue;

        LOG_DEBUG << thisObject_.GetName() << " Load model: " << file.GetBaseName();
        auto model = componentContext_.resourceManager_.LoadModel(file, loadingParameters_);
        if (model)
        {
            auto existModel = model_.Get(lvl);
            if (not existModel)
            {
                model_.Add(model, lvl);
            }
            else
            {
                model_.Update(model, lvl);
                ReleaseCustomMaterials(*existModel);
                componentContext_.resourceManager_.ReleaseModel(*existModel);
            }
        }
        else
        {
            LOG_ERROR << thisObject_.GetName() << " model load error: " << file.GetBaseName();
        }
    }

    bool atLeastOneModel = false;
    for (uint32 i = 0; i < magic_enum::enum_count<LevelOfDetail>(); ++i)
    {
        if (auto lvl = magic_enum::enum_cast<LevelOfDetail>(i))
        {
            if (auto model = model_.Get(*lvl))
            {
                // ReserveBufferVectors(model->GetMeshes().size());
                CreateBuffers(*model);
                PrepareCustomMaterials(*model);
                atLeastOneModel = true;
            }
        }
    }

    if (atLeastOneModel)
    {
        Subscribe();
    }
}

void RendererComponent::PrepareCustomMaterials(const Model& model)
{
    for (auto& mesh : model.GetMeshes())
    {
        if (auto iter = materials.find(mesh.GetMaterial().name); iter == materials.end())
        {
            materials.insert({mesh.GetMaterial().name, {}});
        }
        else if (not iter->second.empty() and Utils::toLower(iter->second.GetAbsolutePath().string()) != "default")
        {
            LOG_DEBUG << Utils::toLower(iter->second.GetAbsolutePath().string());
            customMaterials.try_emplace(mesh.GetGpuObjectId(), componentContext_.graphicsApi_,
                                        componentContext_.gpuResourceLoader_,
                                        ParseMaterial(iter->second, componentContext_.resourceManager_.GetTextureLoader()));
        }
    }
}

void RendererComponent::AddCustomMaterial(const Material& material)
{
    auto model = model_.Get(L1);
    if (not model)
        return;
    for (auto& mesh : model->GetMeshes())
    {
        AddCustomMaterial(mesh, material);
    }
}

void RendererComponent::AddCustomMaterial(const Mesh& mesh, const Material& material)
{
    customMaterials.try_emplace(mesh.GetGpuObjectId(), componentContext_.graphicsApi_, componentContext_.gpuResourceLoader_,
                                material);
}

void RendererComponent::UpdateCustomMaterial(const Mesh& mesh, const Material& material)
{
    customMaterials.erase(mesh.GetGpuObjectId());
    AddCustomMaterial(mesh, material);
}

void RendererComponent::ReleaseCustomMaterials(const Model& model)
{
    for (auto& mesh : model.GetMeshes())
    {
        customMaterials.erase(mesh.GetGpuObjectId());
    }
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
        LOG_ERROR << "perObjectUpdateBuffer object already exist!";
        return;
    }

    auto& graphicsApi = componentContext_.graphicsApi_;

    auto bufferPtr = std::make_unique<BufferObject<PerObjectUpdate>>(graphicsApi, PER_OBJECT_UPDATE_BIND_LOCATION);
    auto& buffer   = *bufferPtr.get();
    perObjectUpdateBuffer_.insert({mesh.GetGpuObjectId(), std::move(bufferPtr)});

    const mat4 transformMatrix            = thisObject_.GetWorldTransform().CalculateCurrentMatrix() * mesh.GetMeshTransform();
    buffer.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(transformMatrix);
    LOG_DEBUG << "bufferId=" << buffer.GetGpuObjectId();
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(buffer);
    calculateWorldSpaceBoundingBox(thisObject_.GetWorldTransform().CalculateCurrentMatrix());
}
void RendererComponent::CreatePerObjectConstantsBuffer(const Mesh& mesh)
{
    auto iter = perObjectConstantsBuffer_.find(mesh.GetGpuObjectId());
    if (iter != perObjectConstantsBuffer_.end())
    {
        LOG_ERROR << "perObjectConstantsBuffer object already exist!";
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

    LOG_DEBUG << "bufferId=" << buffer.GetGpuObjectId();
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
                componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(buffer);
                calculateWorldSpaceBoundingBox(thisObject_.GetWorldTransform().CalculateCurrentMatrix());
            }
            else
            {
                LOG_ERROR << "perObjectUpdateBuffer not found! : " << thisObject_.GetName() << ", id : " << thisObject_.GetId();
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
                LOG_ERROR << "perObjectUpdateBuffer not found! : " << thisObject_.GetName() << ", id : " << thisObject_.GetId();
            }
        }
    }
}

const std::unordered_map<IdType, CustomMaterialData>& RendererComponent::GetCustomMaterials() const
{
    return customMaterials;
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
                LOG_ERROR << "SetTextureIndex index error";
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
                            component->fileName_LOD1 = filename;
                        }
                        else if (lodInt == 1)
                        {
                            component->fileName_LOD2 = filename;
                        }
                        else if (lodInt == 2)
                        {
                            component->fileName_LOD3 = filename;
                        }
                        else
                        {
                            LOG_ERROR << "LOD \"" + std::to_string(lodInt) + "\" is out of range. Correct range is 0-2";
                        }
                    }
                    catch (...)
                    {
                        LOG_ERROR << "Set model filenames error";
                    }
                }
            }
        }

        auto primitivesNode = node.getChild(CSTR_PRIMITIVES);
        if (primitivesNode)
        {
            for (auto& primitiveNode : primitivesNode->getChildren())
            {
                if (primitiveNode)
                {
                    try
                    {
                        auto type = magic_enum::enum_cast<PrimitiveType>(primitiveNode->value_);
                        if (type)
                        {
                            auto primitive = componentContext.resourceManager_.GetPrimitives(*type);
                            if (primitive)
                            {
                                component->AddModel(primitive);
                            }
                        }
                        else
                        {
                            LOG_ERROR << "Primitive type \"" + primitiveNode->value_ + "\" not recognized.";
                        }
                    }
                    catch (...)
                    {
                        LOG_ERROR << "Set primitive error";
                    }
                }
            }
        }

        auto materialsNode = node.getChild(MATERIALS);
        if (materialsNode)
        {
            for (const auto& materialNode : materialsNode->getChildren())
            {
                auto nameNode = materialNode->getChild(CSTR_NAME);
                auto fileNode = materialNode->getChild(CSTR_FILE_NAME);
                if (nameNode and fileNode)
                {
                    component->materials.insert({nameNode->value_, fileNode->value_});
                }
            }
        }

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<RendererComponent>(), readFunc);
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

void create(TreeNode& materialsNode, const MaterialsMap& customMaterials)
{
    for (const auto& [name, file] : customMaterials)
    {
        auto& materialNode = materialsNode.addChild("material");
        materialNode.addChild(CSTR_NAME, name);
        auto value = file.empty() ? "" : file.GetDataRelativePath();
        materialNode.addChild(CSTR_FILE_NAME, value);
    }
}

void RendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    node.addChild(CSTR_TEXTURE_INDEX, std::to_string(textureIndex));
    node.addChild(MODEL_NORMALIZATION, Utils::BoolToString(modelNormalization));
    node.addChild(MESH_OPTIMIZE, Utils::BoolToString(meshOptimize));

    create(node.addChild(CSTR_MODEL_FILE_NAMES), GetFiles());
    create(node.addChild(MATERIALS), materials);

    std::vector<Primitive*> primitives;
    for (auto& model : addedModels)
    {
        if (auto primitive = dynamic_cast<Primitive*>(model))
        {
            primitives.push_back(primitive);
        }
    }

    if (not primitives.empty())
    {
        auto& primitivesNode = node.addChild(CSTR_PRIMITIVES);
        for (auto primitive : primitives)
        {
            primitivesNode.addChild(CSTR_PRIMITIVE, magic_enum::enum_name(primitive->GetType()));
        }
    }
}
const GraphicsApi::ID& RendererComponent::GetPerObjectUpdateBuffer(IdType meshId) const
{
    auto iter = perObjectUpdateBuffer_.find(meshId);
    if (iter != perObjectUpdateBuffer_.end())
    {
        return iter->second ? iter->second->GetGraphicsObjectId() : defaultId;
    }

    return defaultId;
}
const GraphicsApi::ID& RendererComponent::GetPerObjectConstantsBuffer(IdType meshId) const
{
    auto iter = perObjectConstantsBuffer_.find(meshId);
    if (iter != perObjectConstantsBuffer_.end())
    {
        return iter->second ? iter->second->GetGraphicsObjectId() : defaultId;
    }

    return defaultId;
}

void RendererComponent::calculateWorldSpaceBoundingBox(const mat4& worldMatrix)
{
    if (auto model = model_.Get())
    {
        boundingBox = model->transformBoundingBox(worldMatrix);
    }
}

const BoundingBox& RendererComponent::getWorldSpaceBoundingBox() const
{
    return boundingBox;
}
}  // namespace Components
}  // namespace GameEngine
