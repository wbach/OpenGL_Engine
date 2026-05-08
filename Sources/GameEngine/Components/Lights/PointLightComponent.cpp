#include "PointLightComponent.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentCore.h"
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

REGISTER_COMPONENT(PointLightComponent)

PointLightComponent::PointLightComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
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

void PointLightComponent::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_COLOR), color);
    ::Read(node.getChild(CSTR_INTENSITY), intensity);
    ::Read(node.getChild(CSTR_RANGE), range);
    ::Read(node.getChild(CSTR_FALLOFF_EXP), falloffExponent);
}
void PointLightComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_COLOR), color.value);
    ::write(node.addChild(CSTR_INTENSITY), intensity);
    ::write(node.addChild(CSTR_RANGE), range);
    ::write(node.addChild(CSTR_FALLOFF_EXP), falloffExponent);
}
}  // namespace Components
}  // namespace GameEngine
