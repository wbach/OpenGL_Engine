#include "PreviewComponent.h"

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
ComponentsType PreviewComponent::type = ComponentsType::PreviewComponent;

PreviewComponent::PreviewComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(PreviewComponent::type, componentContext, gameObject)
    , isSubscribed_(false)
{
}
PreviewComponent::~PreviewComponent()
{
}
void PreviewComponent::CleanUp()
{
    UnSubscribe();

    for (auto model : model_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
}
void PreviewComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&PreviewComponent::Subscribe, this));
}
void PreviewComponent::addModel(const File& filename, const File& outputFile)
{
    if (filename.empty() or outputFile.empty())
        return;

    outputFile_ = outputFile;

    auto model = componentContext_.resourceManager_.LoadModel(filename);

    if (model)
    {
        auto existModel = model_.Get(LevelOfDetail::L1);
        if (not existModel)
        {
            model_.Add(model, LevelOfDetail::L1);
        }
    }
}
void PreviewComponent::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void PreviewComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
const File& PreviewComponent::getOutputFile() const
{
    return outputFile_;
}
}  // namespace Components
}  // namespace GameEngine
