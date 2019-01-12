#include "BaseComponent.h"

namespace GameEngine
{
namespace Components
{
BaseComponent::BaseComponent(ComponentsType type, const ComponentContext& componentContext, GameObject& gameObject)
    : type_(type)
    , thisObject_(gameObject)
    , componentContext_(componentContext)
{
}
BaseComponent::~BaseComponent()
{
    for (auto id : ids_)
        componentContext_.componentController_.UnRegisterFunction(id.second, id.first);
}
}  // Components
}  // GameEngine
