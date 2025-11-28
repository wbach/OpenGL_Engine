#include "SpotLightComponent.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_DIRECTION[]    = "direction";
constexpr char CSTR_COLOR[]        = "color";
constexpr char CSTR_INTENSITY[]    = "intensity";
constexpr char CSTR_RANGE[]        = "range";
constexpr char CSTR_FALLOFF_EXP[]  = "falloffExponent";
constexpr char CSTR_INNER_CUTOFF[] = "innerCutoff";
constexpr char CSTR_OUTER_CUTOFF[] = "outerCutoff";
}  // namespace

SpotLightComponent::SpotLightComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<SpotLightComponent>(), componentContext, gameObject)
    , color(Color(1.0f, 1.0f, 1.0f))
    , intensity(1.0f)
    , direction(vec3(0.0f, -1.0f, 0.0f))
    , range(10.0f)
    , falloffExponent(2.0f)  // smooth quadratic
    , innerCutoff(std::cos(glm::radians(20.0f)))
    , outerCutoff(std::cos(glm::radians(30.0f)))
{
}

void SpotLightComponent::ReqisterFunctions()
{
}

void SpotLightComponent::CleanUp()
{
}

void SpotLightComponent::Reload()
{
}

void SpotLightComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<SpotLightComponent>(componentContext, gameObject);

        ::Read(node.getChild(CSTR_COLOR), component->color);
        ::Read(node.getChild(CSTR_INTENSITY), component->intensity);
        ::Read(node.getChild(CSTR_DIRECTION), component->direction);
        ::Read(node.getChild(CSTR_RANGE), component->range);
        ::Read(node.getChild(CSTR_FALLOFF_EXP), component->falloffExponent);
        ::Read(node.getChild(CSTR_INNER_CUTOFF), component->innerCutoff);
        ::Read(node.getChild(CSTR_OUTER_CUTOFF), component->outerCutoff);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<SpotLightComponent>(), readFunc);
}

void SpotLightComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    ::write(node.addChild(CSTR_COLOR), color.value);
    ::write(node.addChild(CSTR_INTENSITY), intensity);
    ::write(node.addChild(CSTR_DIRECTION), direction);
    ::write(node.addChild(CSTR_RANGE), range);
    ::write(node.addChild(CSTR_FALLOFF_EXP), falloffExponent);
    ::write(node.addChild(CSTR_INNER_CUTOFF), innerCutoff);
    ::write(node.addChild(CSTR_OUTER_CUTOFF), outerCutoff);
}

}  // namespace Components
}  // namespace GameEngine
