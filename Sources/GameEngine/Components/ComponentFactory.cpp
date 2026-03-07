#include "ComponentFactory.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include "ComponentsReadFunctions.h"
#include "GameEngine/Components/ComponentContext.h"
#include "IComponent.h"
#include "UnknownExternalComponent.h"

namespace GameEngine
{
namespace Components
{
ComponentFactory::ComponentFactory(ComponentContext& context)
    : context_(context)
{
}
std::unique_ptr<IComponent> ComponentFactory::Create(const TreeNode& node, GameObject& gameObject)
{
    const std::string CSTR_TYPE = "type";
    auto componentName          = node.getAttributeValue(CSTR_TYPE);
    if (not componentName.empty())
    {
        if (auto creationFunc = ReadFunctions::instance().get(componentName))
        {
            auto component = creationFunc(context_, node, gameObject);
            component->Register();
            return component;
        }

        LOG_WARN << "Read function not find for component name : " << componentName;
    }
    else
    {
        LOG_ERROR << "Component type not found in component node attribute.";
    }

    if (auto creationFunc = ReadFunctions::instance().get(GetComponentType<UnknownExternalComponent>().name))
    {
        auto component = creationFunc(context_, node, gameObject);
        component->Register();
        return component;
    }

    return nullptr;
}
}  // namespace Components
}  // namespace GameEngine
