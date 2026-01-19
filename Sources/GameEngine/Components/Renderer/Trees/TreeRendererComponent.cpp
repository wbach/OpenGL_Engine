#include "TreeRendererComponent.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/TreeNode.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
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
#include "TreeUtils.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_TRUNK_FILENAMES[]          = "trunkModelFileNames";
constexpr char CSTR_LEAFS_FILENAMES[]          = "leafModelFileNames";
constexpr char CSTR_LEAFS_BILBOARDS_FILENAME[] = "leafBilboardsModelFileNames";
constexpr char CSTR_INSTANCES_POSITIONS[]      = "insatncesPositions";
}  // namespace

TreeRendererComponent::TreeRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<TreeRendererComponent>(), componentContext, gameObject)
    , leafScale{0.75f}
    , leafTextureAtlasSize{3}
    , leafTextureIndex{0}
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

    switch (i)
    {
        case LevelOfDetail::L1:
            trunkModelLod1 = file;
            break;
        case LevelOfDetail::L2:
            trunkModelLod2 = file;
            break;
        case LevelOfDetail::L3:
            trunkModelLod3 = file;
            break;
    }

    LOG_DEBUG << "SetTrunkModel " << magic_enum::enum_name(i) << ", file: " << file;

    auto modelPtr = componentContext_.resourceManager_.LoadModel(file);

    LOG_DEBUG << "SetTrunkModel load done " << magic_enum::enum_name(i) << ", file: " << file;
    thisObject_.TakeWorldTransfromSnapshot();
    trunkModel.Add(modelPtr, i);
    UpdateBoundingBox();
    return *this;
}

TreeRendererComponent& TreeRendererComponent::SetGeneratedTrunkModel(Model* modelPtr, GameEngine::LevelOfDetail i)
{
    auto file = EngineConf.files.getGeneratedDirPath() / ("TreeTrunk_" + Utils::CreateUniqueFilename() + ".bin");

    switch (i)
    {
        case LevelOfDetail::L1:
            trunkModelLod1 = file;
            break;
        case LevelOfDetail::L2:
            trunkModelLod2 = file;
            break;
        case LevelOfDetail::L3:
            trunkModelLod3 = file;
            break;
    }

    generatedModels.insert({i, modelPtr});

    trunkModel.Add(modelPtr, i);
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

        if (not leafBilboardsModel and leafsBilboards.exist())
        {
            LOG_DEBUG << "Loading bilboard model";
            leafBilboardsModel = componentContext_.resourceManager_.LoadModel(leafsBilboards);
        }

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
    for (auto model : trunkModel.PopModels())
        componentContext_.resourceManager_.ReleaseModel(*model);
}
void TreeRendererComponent::DeleteShaderBuffers()
{
    if (perObjectUpdateBuffer_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(perObjectUpdateBuffer_));
    if (perInstances_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(perInstances_));
    if (leafsSsbo_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(leafsSsbo_));
}
void TreeRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<TreeRendererComponent>(componentContext, gameObject);

        auto setModelFiles = [&](const std::string& nodeName, std::function<void(const File& file, LevelOfDetail i)> setModel)
        {
            if (auto filesNode = node.getChild(nodeName))
            {
                for (const auto& fileNode : filesNode->getChildren())
                {
                    auto fileNameNode    = fileNode->getChild(CSTR_FILE_NAME);
                    auto lvlOfDetailNode = fileNode->getChild(CSTR_MODEL_LVL_OF_DETAIL);

                    if (fileNameNode and lvlOfDetailNode and not lvlOfDetailNode->value_.empty())
                    {
                        const auto& filename = fileNameNode->value_;
                        try
                        {
                            if (auto lod = magic_enum::enum_cast<LevelOfDetail>(lvlOfDetailNode->value_))
                            {
                                setModel(filename, *lod);
                            }
                        }
                        catch (...)
                        {
                            LOG_DEBUG << "lod read error. Lod str value = " << lvlOfDetailNode->value_;
                        }
                    }
                }
            }
        };

        setModelFiles(CSTR_TRUNK_FILENAMES, [&](const File& file, LevelOfDetail i) { component->SetTrunkModel(file, i); });

        if (auto filesNode = node.getChild(CSTR_LEAFS_FILENAMES))
        {
            for (const auto& fileNode : filesNode->getChildren())
            {
                auto fileNameNode    = fileNode->getChild(CSTR_FILE_NAME);
                auto lvlOfDetailNode = fileNode->getChild(CSTR_MODEL_LVL_OF_DETAIL);

                if (fileNameNode and lvlOfDetailNode and not lvlOfDetailNode->value_.empty())
                {
                    const auto& filename = fileNameNode->value_;
                    try
                    {
                        if (auto lod = magic_enum::enum_cast<LevelOfDetail>(lvlOfDetailNode->value_))
                        {
                            // TO DO
                            if (lod == LevelOfDetail::L1)
                            {
                                component->leafsFileLod1 = filename;

                                auto [leafSsbo, material] = ImportLeafSSBO(componentContext.resourceManager_.GetTextureLoader(),
                                                                           component->leafsFileLod1.GetAbsolutePath());
                                component->leafMaterial   = material;
                                component->UpdateLeafsSsbo(std::move(leafSsbo));
                            }
                        }
                    }
                    catch (...)
                    {
                        LOG_DEBUG << "lod read error. Lod str value = " << lvlOfDetailNode->value_;
                    }
                }
            }
        }

        if (auto tmpNode = node.getChild(CSTR_LEAFS_BILBOARDS_FILENAME))
        {
            if (auto modelTmpNode = tmpNode->getChild(CSTR_MODEL_FILE_NAME))
            {
                component->leafsBilboards = modelTmpNode->value_;
            }
        }

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

    auto& trunkFilenames = node.addChild(CSTR_TRUNK_FILENAMES);

    auto addModelNode = [](TreeNode& node, const ModelWrapper& model, const File& file, LevelOfDetail lvl)
    {
        auto& lvl1Node = node.addChild(CSTR_MODEL_FILE_NAME);
        lvl1Node.addChild(CSTR_FILE_NAME, file.GetDataRelativePath());
        lvl1Node.addChild(CSTR_MODEL_LVL_OF_DETAIL, magic_enum::enum_name(lvl));

        const Model* modelPtr = model.Get(lvl);
        if (modelPtr and not file.exist())
        {
            ExportModelBinary(*modelPtr, file.GetAbsolutePath());
        }
    };

    addModelNode(trunkFilenames, trunkModel, trunkModelLod1, LevelOfDetail::L1);
    addModelNode(trunkFilenames, trunkModel, trunkModelLod2, LevelOfDetail::L2);
    addModelNode(trunkFilenames, trunkModel, trunkModelLod3, LevelOfDetail::L3);

    auto& leafsFilenames = node.addChild(CSTR_LEAFS_FILENAMES);
    auto& lvl1Node       = leafsFilenames.addChild(CSTR_MODEL_FILE_NAME);
    lvl1Node.addChild(CSTR_FILE_NAME, leafsFileLod1.GetDataRelativePath());
    lvl1Node.addChild(CSTR_MODEL_LVL_OF_DETAIL, magic_enum::enum_name(LevelOfDetail::L1));

    if (leafsSsbo_)
    {
        ExportLeafSSBO(leafsSsbo_->GetData(), leafMaterial, leafsFileLod1.GetAbsolutePath());
    }

    auto& leafsBibloardsNode = node.addChild(CSTR_LEAFS_BILBOARDS_FILENAME);
    leafsBibloardsNode.addChild(CSTR_MODEL_FILE_NAME, leafsBilboards.GetDataRelativePath());

    if (leafBilboardsModel and not leafsBilboards.exist())
    {
        LOG_DEBUG << "ExportModelBinary " << leafsBilboards;
        ExportModelBinary(*leafBilboardsModel, leafsBilboards.GetAbsolutePath());
        LOG_DEBUG << "ExportModelBinary " << leafsBilboards << " done";
    }

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
    return trunkModel;
}
const std::vector<vec3>& TreeRendererComponent::GetInstancesPositions() const
{
    return instancesPositions_;
}
void TreeRendererComponent::UpdateBoundingBox()
{
    // TO DO : calculate leafmodel +  trunkModel
    auto modelPtr = trunkModel.Get();

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
    leafMaterial = material;
    return *this;
}
void TreeRendererComponent::CreateLeafsSsbo()
{
    if (not leafsSsbo_)
    {
        LOG_DEBUG << "Create shaderStorageVectorBufferObject";
        leafsSsbo_ = std::make_unique<ShaderStorageVectorBufferObject<LeafSSBO>>(
            componentContext_.resourceManager_.GetGraphicsApi(), PER_INSTANCES_BIND_LOCATION);
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
    return leafMaterial;
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
    if (leafsFileLod1.empty())
    {
        leafsFileLod1 = EngineConf.files.getGeneratedDirPath() / ("TreeLeaf_" + Utils::CreateUniqueFilename() + ".bin");
    }

    if (not leafsSsbo_)
    {
        CreateLeafsSsbo();
    }

    leafsSsbo_->GetData() = std::move(v);
    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*leafsSsbo_);
}
TreeRendererComponent& TreeRendererComponent::SetLeafBilboardsModel(Model* model)
{
    leafsBilboards = EngineConf.files.getGeneratedDirPath() / ("TreeLeafsBilboards_" + Utils::CreateUniqueFilename() + ".bin");
    leafBilboardsModel = model;
    return *this;
}
const Model* TreeRendererComponent::GetLeafBilboardsModel() const
{
    return leafBilboardsModel;
}
void TreeRendererComponent::SetClusterTextures(const ClusterTextures& textures)
{
    clusterTextures = textures;
}
void TreeRendererComponent::SetTreeClusters(const TreeClusters& clusters)
{
    treeClusters = clusters;
}
const ClusterTextures& TreeRendererComponent::getClusterTextures() const
{
    return clusterTextures;
}
const TreeClusters& TreeRendererComponent::getTreeClusters() const
{
    return treeClusters;
}
}  // namespace Components
}  // namespace GameEngine
