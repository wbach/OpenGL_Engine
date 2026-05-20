#include "HealthComponent.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "CharacterStats/CharacterStatsComponent.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
REGISTER_COMPONENT(HealthComponent)

HealthComponent::HealthComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}

HealthComponent::~HealthComponent()
{
}

void HealthComponent::CleanUp()
{
}

void HealthComponent::Reload()
{
}

void HealthComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         stats      = thisObject_.GetComponent<CharacterStatsComponent>();
                         controller = thisObject_.GetComponent<CharacterController>();
                     });
}

void HealthComponent::takeDamage(float rawDamage)
{
    if (not canProccess())
    {
        return;
    }

    auto& attributes = stats->attributes;

    auto defense     = attributes.str.getValue();
    auto finalDamage = std::max(1.0f, rawDamage - defense);

    if (attributes.currentLife > finalDamage)
    {
        attributes.currentLife -= finalDamage;
        LOG_DEBUG << "HP: " << attributes.currentLife;
        controller->pushEventToQueue(HurtEvent{});
    }
    else
    {
        attributes.currentLife = 0;
        if (controller)
        {
            LOG_INFO << thisObject_.GetName() << " has died.";
            controller->pushEventToQueue(DeathEvent{});
        }
    }
}

void HealthComponent::heal(float amount)
{
    if (not canProccess())
    {
        return;
    }

    auto& attributes = stats->attributes;
    attributes.currentLife += amount;

    if (attributes.currentLife > attributes.maxLife.getValue())
        attributes.currentLife = attributes.maxLife.getValue();
}

void HealthComponent::read(const TreeNode&)
{
}

void HealthComponent::write(TreeNode& node) const
{
}

bool HealthComponent::canProccess() const
{
    if (not stats)
        return false;

    if (controller and not controller->isAlive())
        return false;

    return true;
}
}  // namespace Components
}  // namespace GameEngine
