#include "CombatStatsComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace
{
inline constexpr char CSTR_MODIFIERS[]{"modifiers"};
inline constexpr char CSTR_MODIFIER[]{"modifier"};
inline constexpr char CSTR_TARGET[]{"target"};
// inline constexpr char CSTR_TYPE[]{"type"};
inline constexpr char CSTR_VALUE[]{"value"};
}  // namespace

namespace Components
{
REGISTER_COMPONENT(CombatStatsComponent)

CombatStatsComponent::CombatStatsComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
    modifiersFields.createElement = [this]()
    {
        modifiers.emplace_back();
        auto& newMod = modifiers.back();

        return CustomStructure{.name   = "ItemStatModifier",
                               .fields = {MakeEnumField("Target", &newMod.target),
                                          MakeEnumField("Type", &newMod.type),
                                          {"Value", FieldType::Float, &newMod.value}}};
    };

    modifiersFields.initElements = [this](std::vector<CustomStructure>& out)
    {
        for (auto& modifier : modifiers)
        {
            out.push_back(CustomStructure{.name   = "ItemStatModifier",
                                          .fields = {MakeEnumField("Target", &modifier.target),
                                                     MakeEnumField("Type", &modifier.type),
                                                     {"Value", FieldType::Float, &modifier.value}}});
        }
    };
}

CombatStatsComponent::~CombatStatsComponent()
{
}

void CombatStatsComponent::CleanUp()
{
}

void CombatStatsComponent::Reload()
{
}

void CombatStatsComponent::ReqisterFunctions()
{
}

void CombatStatsComponent::read(const TreeNode& input)
{
    modifiers.clear();
    if (auto container = input.getChild(CSTR_MODIFIERS))
    {
        for (const auto& child : container->getChildren(CSTR_MODIFIER))
        {
            ItemStatModifier mod;
            ::Read(child->getChild(CSTR_TARGET), mod.target);
            ::Read(child->getChild(CSTR_TYPE), mod.type);
            ::Read(child->getChild(CSTR_VALUE), mod.value);
            modifiers.push_back(mod);
        }
    }

    modifiersFields.init();
}

void CombatStatsComponent::write(TreeNode& node) const
{
    auto& container = node.addChild(CSTR_MODIFIERS);
    for (const auto& mod : modifiers)
    {
        auto& child = container.addChild(CSTR_MODIFIER);
        ::write(child.addChild(CSTR_TARGET), mod.target);
        ::write(child.addChild(CSTR_TYPE), mod.type);
        ::write(child.addChild(CSTR_VALUE), mod.value);
    }
}
}  // namespace Components
}  // namespace GameEngine
