#include "GameState.h"

namespace GameEngine
{
bool GameState::hasFlag(const std::string& flagName) const
{
    return getFlag(flagName).has_value();
}
std::optional<int> GameState::getFlag(const std::string& flagName) const
{
    std::lock_guard lk(observersMutex);
    auto it = flags.find(flagName);
    if (it != flags.end())
    {
        return it->second;
    }
    return std::nullopt;
}
void GameState::setFlag(const std::string& flagName, int value)
{
    std::lock_guard lk(observersMutex);

    flags[flagName] = value;
    auto iter       = observers.find(flagName);
    if (iter != observers.end())
    {
        for (auto& [_, observer] : iter->second)
        {
            observer(flagName, value);
        }
    }
}
IdType GameState::addObserver(const std::string& flagName, FlagCallback callback)
{
    std::lock_guard lk(observersMutex);

    auto id = observerIdPool.getId();
    observers[flagName].insert({id, callback});
    observersIds[id] = flagName;
    return id;
}
void GameState::removeObserver(IdType id)
{
    std::lock_guard lk(observersMutex);

    auto observersIdsIter = observersIds.find(id);

    if (observersIdsIter != observersIds.end())
    {
        auto observersIter = observers.find(observersIdsIter->second);
        if (observersIter != observers.end())
        {
            observersIter->second.erase(observersIdsIter->first);
            observerIdPool.releaseId(observersIdsIter->first);
            if (observersIter->second.empty())
            {
                observers.erase(observersIdsIter->second);
            }
        }
    }
}
}  // namespace GameEngine
