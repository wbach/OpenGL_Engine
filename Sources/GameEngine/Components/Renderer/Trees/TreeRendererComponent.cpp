#include "TreeRendererComponent.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/TreeNode.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Renderer/Trees/ClusterData.h"
#include "GameEngine/Components/Renderer/Trees/Leaf.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryExporter.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GraphicsApi/MeshRawData.h"
#include "TreeSerialization.h"
#include "TreeUtils.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_INSTANCES_POSITIONS[] = "insatncesPositions";
constexpr char CSTR_ATLAS_SIZE[]          = "atlasSize";

std::vector<ClusterData> getClusterData(const TreeClusters& treeClusters)
{
    std::vector<ClusterData> gpuData;
    gpuData.reserve(treeClusters.clusters.size());

    for (const auto& cluster : treeClusters.clusters)
    {
        ClusterData data;

        vec3 voxelMin = cluster.minBound;
        vec3 voxelMax = cluster.minBound + treeClusters.voxelSize;

        vec3 center     = (voxelMin + voxelMax) * 0.5f;
        vec3 dimensions = voxelMax - voxelMin;
        float maxDim    = std::max({dimensions.x, dimensions.y, dimensions.z});

        data.center = vec4(center, 1.0f);
        data.size   = vec4(dimensions, maxDim);

        gpuData.push_back(data);
    }

    return gpuData;
}
}  // namespace

TreeRendererComponent::TreeRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<TreeRendererComponent>(), componentContext, gameObject)
    , leafTextureAtlasSize{3}
    , isSubsribed_(false)
{
}
void TreeRendererComponent::CleanUp()
{
    UnSubscribe();
    DeleteShaderBuffers();
    ReleaseModels();
}
void TreeRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TreeRendererComponent::Awake, this));
}

void TreeRendererComponent::Reload()
{
    // TO DO backup of material, generaedModels
    // if (leafMaterialTmp->diffuseTexture->GetFile())
    // {
    // to do only if model material is changed then relase
    //     // clone texture, reouse texture isntace id increase, load texture return exising texutre
    //     leafMaterialTmp->diffuseTexture =
    //     componentContext_.resourceManager_.GetTextureLoader().LoadTexture(*leafMaterialTmp->diffuseTexture->GetFile(),
    //                                                                       TextureParameters{});

    // }

    // CleanUp();
    // Awake();

    // for (auto& [lvl, ptr] : generatedModels) // to do only if model is changed
    // {
    //     SetGeneratedModel(ptr, lvl);
    // }
}

TreeRendererComponent& TreeRendererComponent::SetInstancesPositions(const std::vector<vec3>& positions)
{
    instancesPositions_ = positions;
    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetTrunkModel(const File& file, LevelOfDetail i)
{
    if (file.empty())
        return *this;

    LOG_DEBUG << "SetTrunkModel " << magic_enum::enum_name(i) << ", file: " << file;

    auto modelPtr = componentContext_.resourceManager_.LoadModel(file);

    LOG_DEBUG << "SetTrunkModel load done " << magic_enum::enum_name(i) << ", file: " << file;
    thisObject_.TakeWorldTransfromSnapshot();
    tree.trunkModel.Add(modelPtr, i);
    UpdateBoundingBox();
    return *this;
}

TreeRendererComponent& TreeRendererComponent::SetGeneratedTrunkModel(Model* modelPtr, GameEngine::LevelOfDetail i)
{
    GenerateFileNameIfNeeded();

    generatedModels.insert({i, modelPtr});

    tree.trunkModel.Add(modelPtr, i);
    UpdateBoundingBox();
    return *this;
}

void TreeRendererComponent::Awake()
{
    if (not worldTransformSub_)
    {
        worldTransformSub_ = thisObject_.SubscribeOnWorldTransfomChange(
            [this](const auto& transform) mutable
            {
                if (not perObjectUpdateBuffer_)
                    return;

                perObjectUpdateBuffer_->GetData().TransformationMatrix =
                    componentContext_.graphicsApi_.PrepareMatrixToLoad(transform.CalculateCurrentMatrix());
                componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*perObjectUpdateBuffer_);
                UpdateBoundingBox();
            });
    }

    if (not isSubsribed_)
    {
        CreatePerObjectUpdateBuffer();
        CreatePerInstancesBuffer();
        CreateLeafsSsbo();
        CreateClusterSsbo();

        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubsribed_ = true;
    }

    UpdateBoundingBox();
}
void TreeRendererComponent::UnSubscribe()
{
    if (isSubsribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubsribed_ = false;
    }

    if (worldTransformSub_)
    {
        thisObject_.UnsubscribeOnWorldTransfromChange(*worldTransformSub_);
        worldTransformSub_.reset();
    }
}
void TreeRendererComponent::CreatePerObjectUpdateBuffer()
{
    perObjectUpdateBuffer_ = std::make_unique<ShaderBufferObject<PerObjectUpdate>>(
        componentContext_.resourceManager_.GetGraphicsApi(), PER_OBJECT_UPDATE_BIND_LOCATION);

    perObjectUpdateBuffer_->GetData().TransformationMatrix =
        componentContext_.graphicsApi_.PrepareMatrixToLoad(thisObject_.GetWorldTransform().CalculateCurrentMatrix());

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(*perObjectUpdateBuffer_);
}
void TreeRendererComponent::CreatePerInstancesBuffer()
{
    perInstances_ = std::make_unique<ShaderBufferObject<PerInstances>>(componentContext_.resourceManager_.GetGraphicsApi(),
                                                                       PER_INSTANCES_BIND_LOCATION);

    int index = 0;
    for (const auto& pos : instancesPositions_)
    {
        perInstances_->GetData().transforms[index++] = Utils::CreateTransformationMatrix(pos, DegreesVec3(0.f), vec3(1.f));
    }

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(*perInstances_);
}
void TreeRendererComponent::ReleaseModels()
{
    for (auto model : tree.trunkModel.PopModels())
        componentContext_.resourceManager_.ReleaseModel(*model);

    if (tree.leafBilboardsModel)
    {
        componentContext_.resourceManager_.ReleaseModel(*tree.leafBilboardsModel);
        tree.leafBilboardsModel = nullptr;
    }
}
void TreeRendererComponent::DeleteShaderBuffers()
{
    if (perObjectUpdateBuffer_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(perObjectUpdateBuffer_));
    if (perInstances_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(perInstances_));
    if (leafsSsbo_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(leafsSsbo_));
    if (clustersSsbo_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(clustersSsbo_));
}
void TreeRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<TreeRendererComponent>(componentContext, gameObject);

        if (auto filenameNode = node.getChild(CSTR_FILE_NAME))
        {
            if (not filenameNode->value_.empty())
            {
                component->treeModel = filenameNode->value_;
                component->tree      = Import(componentContext.graphicsApi_, componentContext.resourceManager_,
                                              component->treeModel.GetAbsolutePath());
            }
        }

        ::Read(node.getChild(CSTR_ATLAS_SIZE), component->leafTextureAtlasSize);

        if (auto instancesNode = node.getChild(CSTR_INSTANCES_POSITIONS))
        {
            component->instancesPositions_.clear();
            component->instancesPositions_.reserve(instancesNode->getChildren().size());

            for (auto& child : instancesNode->getChildren())
            {
                vec3 position(0);
                ::Read(child.get(), position);
                component->instancesPositions_.push_back(position);
            }
        }
        LOG_DEBUG << "Read done";
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<TreeRendererComponent>(), readFunc);
}

void TreeRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    node.addChild(CSTR_FILE_NAME, treeModel.GetDataRelativePath());
    if (not treeModel.empty())
    {
        LOG_DEBUG << "Tree all leafs size: " << tree.allLeafs.size();
        Export(tree, treeModel.GetAbsolutePath());
    }

    ::write(node.addChild(CSTR_ATLAS_SIZE), leafTextureAtlasSize);

    if (not instancesPositions_.empty())
    {
        auto& instancesNode = node.addChild(CSTR_INSTANCES_POSITIONS);
        for (const auto& pos : instancesPositions_)
        {
            ::write(instancesNode.addChild(CSTR_POSITION), pos);
        }
    }
}
const ModelWrapper& TreeRendererComponent::GetTrunkModel() const
{
    return tree.trunkModel;
}
const std::vector<vec3>& TreeRendererComponent::GetInstancesPositions() const
{
    return instancesPositions_;
}
void TreeRendererComponent::UpdateBoundingBox()
{
    // TO DO : calculate leafmodel +  trunkModel
    auto modelPtr = tree.trunkModel.Get();

    if (not modelPtr)
        return;

    wolrdModelBoundingBox = modelPtr->getBoundingBox();
    wolrdModelBoundingBox.scale(thisObject_.GetWorldTransform().GetScale());
    wolrdModelBoundingBox.translate(thisObject_.GetWorldTransform().GetPosition());
}
const BoundingBox& TreeRendererComponent::GetWorldBoundingBox() const
{
    return wolrdModelBoundingBox;
}

uint32 TreeRendererComponent::GetInstancesSize() const
{
    return static_cast<uint32>(instancesPositions_.size());
}

const GraphicsApi::ID& TreeRendererComponent::GetPerObjectUpdateId() const
{
    return perObjectUpdateBuffer_->GetGraphicsObjectId();
}
const GraphicsApi::ID& TreeRendererComponent::GetPerInstancesBufferId() const
{
    return perInstances_->GetGraphicsObjectId();
}
TreeRendererComponent& TreeRendererComponent::SetLeafMaterial(const Material& material)
{
    tree.leafMaterial = material;
    return *this;
}
void TreeRendererComponent::CreateLeafsSsbo()
{
    if (not leafsSsbo_)
    {
        LOG_DEBUG << "Create leafs shaderStorageVectorBufferObject";
        leafsSsbo_ = std::make_unique<ShaderStorageVectorBufferObject<LeafSSBO>>(
            componentContext_.resourceManager_.GetGraphicsApi(), PER_INSTANCES_BIND_LOCATION);
    }

    if (leafsSsbo_ and not tree.allLeafs.empty())
    {
        leafsSsbo_->InsertData(tree.allLeafs);
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*leafsSsbo_);
    }
}
const GraphicsApi::ID& TreeRendererComponent::GetLeafsShaderBufferId() const
{
    if (not leafsSsbo_)
    {
        static GraphicsApi::ID id;
        return id;
    }

    return leafsSsbo_->GetGraphicsObjectId();
}
const Material& TreeRendererComponent::GetLeafMaterial() const
{
    return tree.leafMaterial;
}
size_t TreeRendererComponent::GetLeafsCount() const
{
    if (not leafsSsbo_)
    {
        return 0;
    }
    return leafsSsbo_->GetData().size();
}
void TreeRendererComponent::UpdateLeafsSsbo(std::vector<LeafSSBO>&& v)
{
    GenerateFileNameIfNeeded();

    if (not leafsSsbo_)
    {
        CreateLeafsSsbo();
    }

    tree.allLeafs = std::move(v);
    leafsSsbo_->InsertData(tree.allLeafs);
    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*leafsSsbo_);
}
TreeRendererComponent& TreeRendererComponent::SetLeafBilboardsModel(Model* model)
{
    GenerateFileNameIfNeeded();

    tree.leafBilboardsModel = model;
    return *this;
}
const Model* TreeRendererComponent::GetLeafBilboardsModel() const
{
    return tree.leafBilboardsModel;
}
void TreeRendererComponent::SetClusterTextures(const ClusterTextures& textures)
{
    tree.clusters.clusterTextures = textures;
}
void TreeRendererComponent::SetTreeClusters(const TreeClusters& clusters)
{
    tree.clusters.treeClusters = clusters;
    CreateClusterSsbo();
}
const ClusterTextures& TreeRendererComponent::getClusterTextures() const
{
    return tree.clusters.clusterTextures;
}
const TreeClusters& TreeRendererComponent::getTreeClusters() const
{
    return tree.clusters.treeClusters;
}
void TreeRendererComponent::GenerateFileNameIfNeeded()
{
    if (treeModel.empty())
    {
        treeModel = EngineLocalConf.files.getGeneratedDirPath() / ("TreeAllInOne_" + Utils::CreateUniqueFilename() + ".bin");
    }
}
const GraphicsApi::ID& TreeRendererComponent::GetClusterShaderBufferId() const
{
    if (not clustersSsbo_)
    {
        static GraphicsApi::ID id;
        return id;
    }

    return clustersSsbo_->GetGraphicsObjectId();
}
void TreeRendererComponent::CreateClusterSsbo()
{
    GenerateFileNameIfNeeded();

    if (not clustersSsbo_)
    {
        LOG_DEBUG << "Create clusters shaderStorageVectorBufferObject";
        clustersSsbo_ = std::make_unique<ShaderStorageVectorBufferObject<ClusterData>>(
            componentContext_.resourceManager_.GetGraphicsApi(), PER_INSTANCES_BIND_LOCATION);
    }

    if (clustersSsbo_ and not tree.clusters.treeClusters.clusters.empty())
    {
        clustersSsbo_->InsertData(getClusterData(tree.clusters.treeClusters));
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*clustersSsbo_);
    }
}
}  // namespace Components
}  // namespace GameEngine
