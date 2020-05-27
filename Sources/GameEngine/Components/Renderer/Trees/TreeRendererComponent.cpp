#include "TreeRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TreeRendererComponent::type = ComponentsType::TreeRenderer;

TreeRendererComponent::TreeRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::TreeRenderer, componentContext, gameObject)
    , isSubsribed_(false)
{
}
TreeRendererComponent::~TreeRendererComponent()
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
        perInstances_->GetData().transformationMatrixes[index++] =
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
}  // namespace Components
}  // namespace GameEngine
