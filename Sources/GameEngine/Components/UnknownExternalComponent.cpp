#include "UnknownExternalComponent.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "TreeNode.h"

namespace GameEngine
{
namespace Components
{
UnknownExternalComponent::UnknownExternalComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<UnknownExternalComponent>(), componentContext, gameObject)
{
}

void UnknownExternalComponent::CleanUp()
{
}

void UnknownExternalComponent::ReqisterFunctions()
{
}

void UnknownExternalComponent::Reload()
{
}

std::string UnknownExternalComponent::GetOrginalComponentName() const
{
    if (orginalNode)
    {
        if (auto iter = orginalNode->attributes_.find(CSTR_TYPE); iter != orginalNode->attributes_.end())
            return iter->second;
    }
    return {};
}

void UnknownExternalComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component         = std::make_unique<UnknownExternalComponent>(componentContext, gameObject);
        component->orginalNode = std::make_unique<TreeNode>(node);
        if (auto iter = node.attributes_.find(CSTR_TYPE); iter != node.attributes_.end())
            component->type_.name += " (" + iter->second + ")";
        component->SetActive(false);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<UnknownExternalComponent>(), readFunc);
}

void UnknownExternalComponent::write(TreeNode& node) const
{
    node = *orginalNode;
}
}  // namespace Components
}  // namespace GameEngine
