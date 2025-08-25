#include "SkydomeComponent.h"

#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"Skydome"};
}  // namespace

SkydomeComponent::SkydomeComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(COMPONENT_STR, componentContext, gameObject)
    , model_(nullptr)
    , isSubscribed_(false)
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
void SkydomeComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        return std::make_unique<SkydomeComponent>(componentContext, gameObject);
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void SkydomeComponent::write(TreeNode& node) const
{
    node.attributes_.insert({ CSTR_TYPE, COMPONENT_STR });
}
}  // namespace Components
}  // namespace GameEngine
