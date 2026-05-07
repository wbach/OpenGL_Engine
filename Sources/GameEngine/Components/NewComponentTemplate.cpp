#include "NewComponentTemplate.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

namespace GameEngine
{
namespace Components
{
NewComponentTemplate::NewComponentTemplate(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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
void NewComponentTemplate::read(const TreeNode&)
{
}
void NewComponentTemplate::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    ComponentCore::write(node);
}
}  // namespace Components
}  // namespace GameEngine
