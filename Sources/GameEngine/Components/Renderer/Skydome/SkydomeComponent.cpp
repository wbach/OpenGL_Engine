#include "SkydomeComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType SkydomeComponent::type = ComponentsType::Skydome;

SkydomeComponent::SkydomeComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Skydome, componentContext, gameObject)
{
}
SkydomeComponent::~SkydomeComponent()
{
    UnSubscribe();
    if (model_)
        componentContext_.resourceManager_.ReleaseModel(*model_);
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
    model_ = componentContext_.resourceManager_.LoadModel("Meshes/dome/dome_long.obj");
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void SkydomeComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine
