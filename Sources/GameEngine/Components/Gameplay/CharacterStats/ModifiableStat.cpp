#include "ModifiableStat.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <string>
#include <string_view>

#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace
{
inline constexpr char CSTR_BASE_VALUE[]{"baseValue"};
inline constexpr char CSTR_STAT_MODIFIERS[]{"statModifiers"};
inline constexpr char CSTR_STAT_MODIFIER[]{"statModifier"};
inline constexpr char CSTR_STAT_SOURCE[]{"source"};
inline constexpr char CSTR_STAT_SOURCE_ID[]{"sourceId"};
inline constexpr char CSTR_STAT_TYPE[]{"type"};
inline constexpr char CSTR_STAT_VALUE[]{"value"};
}  // namespace

ModifiableStat::ModifiableStat(float baseVal)
    : baseValue(baseVal)
{
}
float ModifiableStat::getValue() const
{
    auto flatSum    = baseValue;
    auto percentSum = 0.0f;

    for (const auto &mod : modifiers)
    {
        if (mod.type == ModifierType::Flat)
        {
            flatSum += mod.value;
        }
        else if (mod.type == ModifierType::PercentAdditive)
        {
            percentSum += mod.value;
        }
    }

    return flatSum * (1.0f + percentSum);
}
void ModifiableStat::setBaseValue(float value)
{
    baseValue = value;
}
float ModifiableStat::getBaseValue() const
{
    return baseValue;
}
void ModifiableStat::addModifier(StatSource source, IdType sourceId, ModifierType type, float value)
{
    modifiers.push_back({.source = source, .sourceId = sourceId, .type = type, .value = value});
}
void ModifiableStat::removeModifiersFromSource(IdType sourceId)
{
    auto it = std::remove_if(modifiers.begin(), modifiers.end(),
                             [sourceId](const StatModifier &mod) { return mod.sourceId == sourceId; });
    modifiers.erase(it, modifiers.end());
}
void ModifiableStat::clearAllModifiers()
{
    modifiers.clear();
}
const ModifiableStat::Modifiers &ModifiableStat::getModifiers() const
{
    return modifiers;
}

std::ostream &operator<<(std::ostream &os, const StatModifier &stat)
{
    os << "StatModifier";
    os << "{";
    os << "{source: " << magic_enum::enum_name(stat.source) << "}";
    os << "{sourceId: " << stat.sourceId << "}";
    os << "{ModifierType: " << magic_enum::enum_name(stat.type) << "}";
    os << "{value: " << stat.value << "}";
    os << "}";
    return os;
}
std::ostream &operator<<(std::ostream &os, const ModifiableStat &stat)
{
    os << "ModifiableStat";
    os << "{";
    os << "{baseValue: " << stat.getBaseValue() << "}";
    os << "{value: " << stat.getValue() << "}";

    for (const auto &mod : stat.getModifiers())
    {
        os << mod;
    }
    os << "}";
    return os;
}
void ModifiableStat::addModifier(const StatModifier &v)
{
    modifiers.push_back(v);
}
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::ModifiableStat &stat)
{
    return to_string(static_cast<int>(
        std::round(stat.getValue())));  //+ " (Base: " + to_string(static_cast<int>(std::round(stat.getBaseValue()))) + ")";
}
}  // namespace std

void Read(const TreeNode &input, GameEngine::StatModifier &stat)
{
    ::Read(input.getChild(GameEngine::CSTR_STAT_SOURCE), stat.source);
    ::Read(input.getChild(GameEngine::CSTR_STAT_SOURCE_ID), stat.sourceId);
    ::Read(input.getChild(GameEngine::CSTR_STAT_TYPE), stat.type);
    ::Read(input.getChild(GameEngine::CSTR_STAT_VALUE), stat.value);
}

void Read(const TreeNode &input, GameEngine::ModifiableStat &stat)
{
    float v{0.f};
    ::Read(input.getChild(GameEngine::CSTR_BASE_VALUE), v);
    stat.setBaseValue(v);

    if (auto modifires = input.getChild(GameEngine::CSTR_STAT_MODIFIERS))
    {
        for (const auto &child : modifires->getChildren())
        {
            GameEngine::StatModifier statModifier;
            Read(*child, statModifier);
            stat.addModifier(statModifier);
        }
    }
}

void write(TreeNode &node, const GameEngine::StatModifier &stat)
{
    ::write(node.addChild(GameEngine::CSTR_STAT_SOURCE), stat.source);
    ::write(node.addChild(GameEngine::CSTR_STAT_SOURCE_ID), stat.sourceId);
    ::write(node.addChild(GameEngine::CSTR_STAT_TYPE), stat.type);
    ::write(node.addChild(GameEngine::CSTR_STAT_VALUE), stat.value);
}

void write(TreeNode &node, const GameEngine::ModifiableStat &stat)
{
    ::write(node.addChild(GameEngine::CSTR_BASE_VALUE), stat.getBaseValue());

    auto &modifiersNode = node.addChild(GameEngine::CSTR_STAT_MODIFIERS);
    for (const auto &mod : stat.getModifiers())
    {
        write(modifiersNode.addChild(GameEngine::CSTR_STAT_MODIFIER), mod);
    }
}
