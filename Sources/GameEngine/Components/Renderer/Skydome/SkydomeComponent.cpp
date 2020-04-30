#include "SkydomeComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SkydomeComponent::type = ComponentsType::Skydome;

SkydomeComponent::SkydomeComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Skydome, componentContext, gameObject)
    , model_(nullptr)
    , isSubscribed_(false)
{
}
SkydomeComponent::~SkydomeComponent()
{
}

void SkydomeComponent::CleanUp()
{
    UnSubscribe();

    if (model_)
    {
        componentContext_.resourceManager_.ReleaseModel(*model_);
        model_ = nullptr;
    }
}

void SkydomeComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&SkydomeComponent::LoadAndSubscribe, this));
}
Model* SkydomeComponent::GetModel()
{
    return model_;
}
void SkydomeComponent::LoadAndSubscribe()
{
    if (not model_)
    {
        model_ = componentContext_.resourceManager_.LoadModel("Meshes/dome/dome_long.obj");
    }
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void SkydomeComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
}  // namespace Components
}  // namespace GameEngine
