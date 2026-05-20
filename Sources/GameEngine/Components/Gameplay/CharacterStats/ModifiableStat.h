#pragma once
#include <algorithm>
#include <vector>

#include "Types.h"

namespace GameEngine
{
enum class StatSource : uint8_t
{
    Equipment,
    Buff,
    Debuff
};

enum class ModifierType : uint8_t
{
    Flat,
    PercentAdditive
};

struct StatModifier
{
    StatSource source;
    IdType sourceId;
    ModifierType type;
    float value;
};

class ModifiableStat
{
public:
    using Modifiers = std::vector<StatModifier>;

    ModifiableStat(float);

    float getValue() const;
    void setBaseValue(float);
    float getBaseValue() const;
    void addModifier(StatSource, IdType, ModifierType, float);
    void addModifier(const StatModifier &);
    void removeModifiersFromSource(IdType);
    void clearAllModifiers();

    const Modifiers &getModifiers() const;

    float &ptr()
    {
        return baseValue;
    }

private:
    float baseValue{0.0f};
    Modifiers modifiers;
};

std::ostream &operator<<(std::ostream &, const StatModifier &);
std::ostream &operator<<(std::ostream &, const ModifiableStat &);
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::ModifiableStat &);
}

class TreeNode;
ENGINE_API void Read(const TreeNode &, GameEngine::StatModifier &);
ENGINE_API void write(TreeNode &, const GameEngine::StatModifier &);
ENGINE_API void Read(const TreeNode &, GameEngine::ModifiableStat &);
ENGINE_API void write(TreeNode &, const GameEngine::ModifiableStat &);