#pragma once
#include <EngineApi.h>

#include <string>
#include <unordered_map>
namespace GameEngine
{
struct ENGINE_API GameState
{
    std::unordered_map<std::string, int> flags;

    bool hasFlag(const std::string& flagName) const
    {
        auto it = flags.find(flagName);
        if (it != flags.end())
        {
            return it->second > 0;
        }
        return false;
    }

    void setFlag(const std::string& flagName, int value)
    {
        flags[flagName] = value;
    }
};
}  // namespace GameEngine
