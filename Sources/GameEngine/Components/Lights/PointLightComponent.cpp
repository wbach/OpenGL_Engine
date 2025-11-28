#include "PointLightComponent.h"

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
constexpr char CSTR_COLOR[]       = "color";
constexpr char CSTR_INTENSITY[]   = "intensity";
constexpr char CSTR_RANGE[]       = "range";
constexpr char CSTR_FALLOFF_EXP[] = "falloffExponent";
}  // namespace
PointLightComponent::PointLightComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<PointLightComponent>(), componentContext, gameObject)
    , color(Color(1.0f, 1.0f, 1.0f))
    , intensity(1.0f)
    , range(10.0f)
    , falloffExponent(2.0f)
{
}

void PointLightComponent::ReqisterFunctions()
{
}

void PointLightComponent::CleanUp()
{
}

void PointLightComponent::Reload()
{
}

void PointLightComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<PointLightComponent>(componentContext, gameObject);

        ::Read(node.getChild(CSTR_COLOR), component->color);
        ::Read(node.getChild(CSTR_INTENSITY), component->intensity);
        ::Read(node.getChild(CSTR_RANGE), component->range);
        ::Read(node.getChild(CSTR_FALLOFF_EXP), component->falloffExponent);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<PointLightComponent>(), readFunc);
}
void PointLightComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    ::write(node.addChild(CSTR_COLOR), color.value);
    ::write(node.addChild(CSTR_INTENSITY), intensity);
    ::write(node.addChild(CSTR_RANGE), range);
    ::write(node.addChild(CSTR_FALLOFF_EXP), falloffExponent);
}
}  // namespace Components
}  // namespace GameEngine
