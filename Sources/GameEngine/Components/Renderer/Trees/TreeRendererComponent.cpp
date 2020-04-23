#include "TreeRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TreeRendererComponent::type = ComponentsType::TreeRenderer;

TreeRendererComponent::TreeRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::TreeRenderer, componentContext, gameObject)
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

    ModelRawPtr model = componentContext_.resourceManager_.LoadModel(filename);
    thisObject_.TakeWorldTransfromSnapshot();
    bottom_.Add(model, i);

    return *this;
}
TreeRendererComponent& TreeRendererComponent::SetTopModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
    if (filename.empty())
        return *this;

    topFilenames_.insert({filename, i});

    ModelRawPtr model = componentContext_.resourceManager_.LoadModel(filename);
    thisObject_.TakeWorldTransfromSnapshot();
    top_.Add(model, i);

    return *this;
}
void TreeRendererComponent::Subscribe()
{
    if (not positions_.empty())
    {
        CreatePerObjectUpdateBuffer();
        CreatePerInstancesBuffer();

        componentContext_.renderersManager_.Subscribe(&thisObject_);
    }
}
void TreeRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
void TreeRendererComponent::CreatePerObjectUpdateBuffer()
{
    const auto& tmodel = top_.Get(L1);
    const auto& bmodel = bottom_.Get(L1);

    float factor =
        tmodel->GetScaleFactor() > bmodel->GetScaleFactor() ? bmodel->GetScaleFactor() : bmodel->GetScaleFactor();

    perObjectUpdateBuffer_ = std::make_unique<BufferObject<PerObjectUpdate>>(
        componentContext_.resourceManager_.GetGraphicsApi(), PER_OBJECT_UPDATE_BIND_LOCATION);

    auto normalizedMatrix = glm::scale(vec3(1.f / factor)) * thisObject_.GetWorldTransform().GetMatrix();
    perObjectUpdateBuffer_->GetData().TransformationMatrix = normalizedMatrix;

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(perObjectUpdateBuffer_.get());
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

    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(perInstances_.get());
}
}  // namespace Components
}  // namespace GameEngine
