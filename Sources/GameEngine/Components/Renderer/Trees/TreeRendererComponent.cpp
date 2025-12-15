#include "TreeRendererComponent.h"

#include <Utils/GLM/GLMUtils.h>
#include <Utils/TreeNode.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Models/WBLoader/Assimp/AssimpExporter.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Logger/Log.h"
#include "Logger/LoggingLvl.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_FILENAMES[]              = "modelFileNames";
constexpr char CSTR_LEAFS_POSITON_FILENAME[] = "leafPositionFile";
constexpr char CSTR_INSTANCES_POSITIONS[]    = "insatncesPositions";
}  // namespace

TreeRendererComponent::TreeRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<TreeRendererComponent>(), componentContext, gameObject)
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
    RegisterFunction(FunctionType::Awake, std::bind(&TreeRendererComponent::Subscribe, this));
}

void TreeRendererComponent::Reload()
{
    CleanUp();
    Subscribe();
}

TreeRendererComponent& TreeRendererComponent::SetInstancesPositions(const std::vector<vec3>& positions)
{
    instancesPositions_ = positions;
    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetModel(const File& file, LevelOfDetail i)
{
    if (file.empty())
        return *this;

    switch (i)
    {
        case LevelOfDetail::L1:
            modelLod1 = file;
            break;
        case LevelOfDetail::L2:
            modelLod2 = file;
            break;
        case LevelOfDetail::L3:
            modelLod3 = file;
            break;
    }

    auto modelPtr = componentContext_.resourceManager_.LoadModel(file);
    thisObject_.TakeWorldTransfromSnapshot();
    model.Add(modelPtr, i);
    return *this;
}

TreeRendererComponent& TreeRendererComponent::SetGeneratedModel(Model* modelPtr, GameEngine::LevelOfDetail i)
{
    auto file = EngineConf.files.getGeneratedDirPath() / ("Tree_" + Utils::CreateUniqueFilename() + ".obj");

    switch (i)
    {
        case LevelOfDetail::L1:
            modelLod1 = file;
            break;
        case LevelOfDetail::L2:
            modelLod2 = file;
            break;
        case LevelOfDetail::L3:
            modelLod3 = file;
            break;
    }

    model.Add(modelPtr, i);

    UpdateBoundingBox();
    return *this;
}

void TreeRendererComponent::Subscribe()
{
    if (not isSubsribed_)
    {
        CreatePerObjectUpdateBuffer();
        CreatePerInstancesBuffer();

        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubsribed_ = true;
    }

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
    for (auto model : model.PopModels())
        componentContext_.resourceManager_.ReleaseModel(*model);
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

        if (auto filesNode = node.getChild(CSTR_FILENAMES))
        {
            for (const auto& fileNode : filesNode->getChildren())
            {
                auto fileNameNode    = fileNode->getChild(CSTR_FILE_NAME);
                auto lvlOfDetailNode = fileNode->getChild(CSTR_MODEL_LVL_OF_DETAIL);

                if (fileNameNode and lvlOfDetailNode and not lvlOfDetailNode->value_.empty())
                {
                    const auto& filename = fileNameNode->value_;
                    auto lod             = static_cast<LevelOfDetail>(std::stoi(lvlOfDetailNode->value_));
                    component->SetModel(filename, lod);
                }
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

        if (auto leafsFileNode = node.getChild(CSTR_LEAFS_POSITON_FILENAME))
        {
            component->leafPositionsFile = leafsFileNode->value_;
        }

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<TreeRendererComponent>(), readFunc);
}

void TreeRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    auto& filenames = node.addChild(CSTR_FILENAMES);

    auto addModelNode = [this, &filenames](const File& file, LevelOfDetail lvl)
    {
        auto& lvl1Node = filenames.addChild(CSTR_MODEL_FILE_NAME);
        lvl1Node.addChild(CSTR_FILE_NAME, file.GetDataRelativePath());
        lvl1Node.addChild(CSTR_MODEL_LVL_OF_DETAIL, magic_enum::enum_name(lvl));

        const Model* modelPtr = model.Get(lvl);
        if (modelPtr and not file.exist())
        {
            ExportModel(*modelPtr, file);
        }
    };

    addModelNode(modelLod1, LevelOfDetail::L1);
    addModelNode(modelLod2, LevelOfDetail::L2);
    addModelNode(modelLod3, LevelOfDetail::L3);

    ::write(node.addChild(CSTR_LEAFS_POSITON_FILENAME), leafPositionsFile.GetDataRelativePath());

    if (not instancesPositions_.empty())
    {
        auto& instancesNode = node.addChild(CSTR_INSTANCES_POSITIONS);
        for (const auto& pos : instancesPositions_)
        {
            ::write(instancesNode.addChild(CSTR_POSITION), pos);
        }
    }
}
const ModelWrapper& TreeRendererComponent::GetModel() const
{
    return model;
}
const std::vector<vec3>& TreeRendererComponent::GetInstancesPositions() const
{
    return instancesPositions_;
}
void TreeRendererComponent::UpdateBoundingBox()
{
    auto modelPtr = model.Get();

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
}  // namespace Components
}  // namespace GameEngine
