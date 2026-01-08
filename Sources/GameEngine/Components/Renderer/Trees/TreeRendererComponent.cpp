#include "TreeRendererComponent.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/TreeNode.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryExporter.h"
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
constexpr char CSTR_TRUNK_FILENAMES[]     = "trunkModelFileNames";
constexpr char CSTR_LEAFS_FILENAMES[]     = "leafModelFileNames";
constexpr char CSTR_INSTANCES_POSITIONS[] = "insatncesPositions";
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

TreeRendererComponent& TreeRendererComponent::SetGeneratedLeafModel(Model* modelPtr, GameEngine::LevelOfDetail i)
{
    auto file = EngineConf.files.getGeneratedDirPath() / ("TreeLeaf_" + Utils::CreateUniqueFilename() + ".bin");
    switch (i)
    {
        case LevelOfDetail::L1:
            leafModelFileLod1 = file;
            break;
        case LevelOfDetail::L2:
            leafModelFileLod2 = file;
            break;
        case LevelOfDetail::L3:
            leafModelFileLod3 = file;
            break;
    }

    generatedModels.insert({i, modelPtr});
    leafModel.Add(modelPtr, i);
    return *this;
}

TreeRendererComponent& TreeRendererComponent::SetLeafModel(const File& file, LevelOfDetail i)
{
    if (file.empty())
        return *this;

    switch (i)
    {
        case LevelOfDetail::L1:
            leafModelFileLod1 = file;
            break;
        case LevelOfDetail::L2:
            leafModelFileLod2 = file;
            break;
        case LevelOfDetail::L3:
            leafModelFileLod3 = file;
            break;
    }

    LOG_DEBUG << "SetLeafModel " << magic_enum::enum_name(i) << ", file: " << file;

    auto modelPtr = componentContext_.resourceManager_.LoadModel(file);

    LOG_DEBUG << "SetLeafModel  load done" << magic_enum::enum_name(i) << ", file: " << file;
    leafModel.Add(modelPtr, i);
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
    perObjectUpdateBuffer_ = std::make_unique<BufferObject<PerObjectUpdate>>(componentContext_.resourceManager_.GetGraphicsApi(),
                                                                             PER_OBJECT_UPDATE_BIND_LOCATION);

    perObjectUpdateBuffer_->GetData().TransformationMatrix =
        componentContext_.graphicsApi_.PrepareMatrixToLoad(thisObject_.GetWorldTransform().CalculateCurrentMatrix());

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(*perObjectUpdateBuffer_);
}
void TreeRendererComponent::CreatePerInstancesBuffer()
{
    perInstances_ = std::make_unique<BufferObject<PerInstances>>(componentContext_.resourceManager_.GetGraphicsApi(),
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

    LOG_DEBUG << "Release leafModels ";
    for (auto model : leafModel.PopModels())
    {
        LOG_DEBUG << "Release leafModel ";
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
}
void TreeRendererComponent::DeleteShaderBuffers()
{
    if (perObjectUpdateBuffer_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(perObjectUpdateBuffer_));
    if (perInstances_)
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(perInstances_));
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
        setModelFiles(CSTR_LEAFS_FILENAMES, [&](const File& file, LevelOfDetail i) { component->SetLeafModel(file, i); });

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
    addModelNode(leafsFilenames, leafModel, leafModelFileLod1, LevelOfDetail::L1);
    addModelNode(leafsFilenames, leafModel, leafModelFileLod2, LevelOfDetail::L2);
    addModelNode(leafsFilenames, leafModel, leafModelFileLod3, LevelOfDetail::L3);

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
const ModelWrapper& TreeRendererComponent::GetLeafModel() const
{
    return leafModel;
}
TreeRendererComponent& TreeRendererComponent::SetLeafMaterial(const Material& material)
{
    if (auto model = leafModel.Get())
    {
        for (auto& mesh : model->GetMeshes())
        {
            mesh.SetMaterial(material);
        }
    }

    leafMaterialTmp = material;
    return *this;
}
}  // namespace Components
}  // namespace GameEngine
