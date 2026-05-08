#include "UnknownExternalComponent.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "TreeNode.h"

namespace GameEngine
{
namespace Components
{
UnknownExternalComponent::UnknownExternalComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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
void UnknownExternalComponent::read(const TreeNode& node)
{
    orginalNode = std::make_unique<TreeNode>(node);
    if (auto iter = node.attributes_.find(CSTR_TYPE); iter != node.attributes_.end())
        type_.name += " (" + iter->second + ")";
    SetActive(false);
}
void UnknownExternalComponent::writeToNode(TreeNode& node) const
{
    node = *orginalNode;
}
}  // namespace Components
}  // namespace GameEngine
