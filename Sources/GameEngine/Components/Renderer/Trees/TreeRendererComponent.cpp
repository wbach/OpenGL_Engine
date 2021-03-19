#include "TreeRendererComponent.h"

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
const std::string COMPONENT_STR         = "TreeRenderer";
const std::string CSTR_TOP_FILENAMES    = "topFileNames";
const std::string CSTR_BOTTOM_FILENAMES = "bottomFileNames";
const std::string CSTR_SIZE_2D          = "size2d";
const std::string CSTR_POSITIONS        = "positions";
}  // namespace

TreeRendererComponent::TreeRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(TreeRendererComponent).hash_code(), componentContext, gameObject)
    , isSubsribed_(false)
{
}
void TreeRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TreeRendererComponent::Subscribe, this));
}
TreeRendererComponent& TreeRendererComponent::SetPositions(const std::vector<vec3>& positions, const vec2ui& size2d)
{
    positions_ = positions;
    if (size2d.x == 0 || size2d.y == 0)
    {
        auto s  = sqrt(positions.size());
        size2d_ = vec2ui(static_cast<uint32>(std::floor(s)));
    }
    else
    {
        size2d_ = size2d;
    }

    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetBottomModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
    if (filename.empty())
        return *this;

    bottomFilenames_.insert({filename, i});

    auto model = componentContext_.resourceManager_.LoadModel(filename);
    thisObject_.TakeWorldTransfromSnapshot();
    bottom_.Add(model, i);

    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetTopModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
    if (filename.empty())
        return *this;

    topFilenames_.insert({filename, i});

    auto model = componentContext_.resourceManager_.LoadModel(filename);
    thisObject_.TakeWorldTransfromSnapshot();
    top_.Add(model, i);

    return *this;
}
void TreeRendererComponent::Subscribe()
{
    if (not isSubsribed_ and not positions_.empty())
    {
        CreatePerObjectUpdateBuffer();
        CreatePerInstancesBuffer();

        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubsribed_ = true;
    }
}
void TreeRendererComponent::UnSubscribe()
{
    if (isSubsribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubsribed_ = false;
    }
}
void TreeRendererComponent::CreatePerObjectUpdateBuffer()
{
    const auto& tmodel = top_.Get(L1);
    const auto& bmodel = bottom_.Get(L1);

    float factor = tmodel->getBoundingBox().maxScale() > bmodel->getBoundingBox().maxScale()
                       ? bmodel->getBoundingBox().maxScale()
                       : bmodel->getBoundingBox().maxScale();

    perObjectUpdateBuffer_ = std::make_unique<BufferObject<PerObjectUpdate>>(
        componentContext_.resourceManager_.GetGraphicsApi(), PER_OBJECT_UPDATE_BIND_LOCATION);

    auto normalizedMatrix = glm::scale(vec3(1.f / factor)) * thisObject_.GetWorldTransform().GetMatrix();
    perObjectUpdateBuffer_->GetData().TransformationMatrix = normalizedMatrix;

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(*perObjectUpdateBuffer_);
}
void TreeRendererComponent::CreatePerInstancesBuffer()
{
    perInstances_ = std::make_unique<BufferObject<PerInstances>>(componentContext_.resourceManager_.GetGraphicsApi(),
                                                                 PER_INSTANCES_BIND_LOCATION);

    int index = 0;
    for (const auto& pos : positions_)
    {
        perInstances_->GetData().transforms[index++] =
            Utils::CreateTransformationMatrix(pos, DegreesVec3(0.f), vec3(1.f));
    }

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(*perInstances_);
}
void TreeRendererComponent::CleanUp()
{
    UnSubscribe();
    DeleteShaderBuffers();
    ReleaseModels();
}
void TreeRendererComponent::ReleaseModels()
{
    for (auto model : top_.PopModels())
        componentContext_.resourceManager_.ReleaseModel(*model);
    for (auto model : bottom_.PopModels())
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
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<TreeRendererComponent>(componentContext, gameObject);

        auto bottomFilesNode = node.getChild(CSTR_BOTTOM_FILENAMES);
        if (bottomFilesNode)
        {
            for (const auto& fileNode : bottomFilesNode->getChildren())
            {
                auto fileNameNode    = fileNode->getChild(CSTR_FILE_NAME);
                auto lvlOfDetailNode = fileNode->getChild(CSTR_MODEL_LVL_OF_DETAIL);

                if (fileNameNode and lvlOfDetailNode and not lvlOfDetailNode->value_.empty())
                {
                    const auto& filename = fileNameNode->value_;
                    auto lod             = static_cast<LevelOfDetail>(std::stoi(lvlOfDetailNode->value_));
                    component->SetBottomModel(filename, lod);
                }
            }
        }
        auto topFilesNode = node.getChild(CSTR_BOTTOM_FILENAMES);
        {
            for (const auto& fileNode : topFilesNode->getChildren())
            {
                auto fileNameNode    = fileNode->getChild(CSTR_FILE_NAME);
                auto lvlOfDetailNode = fileNode->getChild(CSTR_MODEL_LVL_OF_DETAIL);

                if (fileNameNode and lvlOfDetailNode and not lvlOfDetailNode->value_.empty())
                {
                    const auto& filename = fileNameNode->value_;
                    auto lod             = static_cast<LevelOfDetail>(std::stoi(lvlOfDetailNode->value_));
                    component->SetTopModel(filename, lod);
                }
            }
        }

        vec2ui size2d(0);
        std::vector<vec3> positions;

        ::Read(node.getChild(CSTR_SIZE_2D), size2d);
        ::Read(node.getChild(CSTR_POSITIONS), positions);

        component->SetPositions(positions, size2d);

        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

namespace
{
void write(TreeNode& node, const std::string& filename, LevelOfDetail lvl)
{
    node.addChild(CSTR_FILE_NAME, filename);
    node.addChild(CSTR_MODEL_LVL_OF_DETAIL, std::to_string(static_cast<int>(lvl)));
}

void write(TreeNode& node, const std::unordered_map<std::string, LevelOfDetail>& files)
{
    for (const auto& files : files)
    {
        write(node.addChild(CSTR_MODEL_FILE_NAME), files.first, files.second);
    }
}
}  // namespace

void TreeRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    Components::write(node.addChild(CSTR_TOP_FILENAMES), topFilenames_);
    Components::write(node.addChild(CSTR_BOTTOM_FILENAMES), bottomFilenames_);
    ::write(node.addChild(CSTR_SIZE_2D), size2d_);
    ::write(node.addChild(CSTR_POSITIONS), positions_);
}
}  // namespace Components
}  // namespace GameEngine
