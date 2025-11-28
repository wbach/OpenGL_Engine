#include "DirectionalLightComponent.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Time/DayNightCycle.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_COLOR[]                      = "color";
constexpr char CSTR_INTENSITY[]                  = "intensity";
constexpr char CSTR_DAY_NIGHT_CYCLE_CONTROLLED[] = "isDayNightCycleControlled";
}  // namespace

DirectionalLightComponent::DirectionalLightComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<DirectionalLightComponent>(), componentContext, gameObject)
    , isDayNightCycleControlled(true)
    , color(vec4(1.f, 1.f, 1.f, 1.f))
    , intensity(1.f)
    , isSetInDayNightCycle(false)
{
}
void DirectionalLightComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&DirectionalLightComponent::Init, this));
}
void DirectionalLightComponent::CleanUp()
{
}
void DirectionalLightComponent::Reload()
{
    Init();
}
void DirectionalLightComponent::Init()
{
    if (isDayNightCycleControlled)
    {
        componentContext_.scene_.GetDayNightCycle().SetDirectionalLight(this);
        isSetInDayNightCycle = true;
    }
    else if (isSetInDayNightCycle)
    {
        componentContext_.scene_.GetDayNightCycle().SetDirectionalLight(nullptr);
        isSetInDayNightCycle = false;
    }
}
void DirectionalLightComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<DirectionalLightComponent>(componentContext, gameObject);

        ::Read(node.getChild(CSTR_DAY_NIGHT_CYCLE_CONTROLLED), component->isDayNightCycleControlled);
        ::Read(node.getChild(CSTR_COLOR), component->color);
        ::Read(node.getChild(CSTR_INTENSITY), component->intensity);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<DirectionalLightComponent>(), readFunc);
}
void DirectionalLightComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    ::write(node.addChild(CSTR_DAY_NIGHT_CYCLE_CONTROLLED), isDayNightCycleControlled);
    ::write(node.addChild(CSTR_COLOR), color);
    ::write(node.addChild(CSTR_COLOR), intensity);
}
vec3 DirectionalLightComponent::GetDirection() const
{
    return -1.f * glm::normalize(thisObject_.GetWorldTransform().GetPosition());
}
}  // namespace Components
}  // namespace GameEngine
