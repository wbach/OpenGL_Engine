#include "NewComponentTemplate.h"

#include <Utils/TreeNode.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace Components
{
NewComponentTemplate::NewComponentTemplate(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<NewComponentTemplate>(), componentContext, gameObject)
{
}
NewComponentTemplate::~NewComponentTemplate()
{
}
void NewComponentTemplate::CleanUp()
{
}
void NewComponentTemplate::Reload()
{
}
void NewComponentTemplate::ReqisterFunctions()
{
}
void NewComponentTemplate::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<NewComponentTemplate>(componentContext, gameObject);
        component->read(input);
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<NewComponentTemplate>(), func);
}
void NewComponentTemplate::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);
}
}  // namespace Components
}  // namespace GameEngine
