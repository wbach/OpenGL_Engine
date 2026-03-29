#include "GameState.h"

#include <Logger/Log.h>

#include <mutex>

namespace GameEngine
{
bool GameState::hasFlag(const std::string& flagName) const
{
    return getFlag(flagName).value_or(0) > 0;
}
std::optional<int> GameState::getFlag(const std::string& flagName) const
{
    std::lock_guard lk(flagMutex);
    auto it = flags.find(flagName);
    if (it != flags.end())
    {
        return it->second;
    }
    return std::nullopt;
}
void GameState::setFlag(const std::string& flagName, int value)
{
    setFlagImpl(flagName, value);
    notifyObservers(flagName, value);
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
    std::lock_guard lk(observersToRemoveMutex);
    observersToRemove.insert(id);
}
void GameState::setFlagImpl(const std::string& flagName, int value)
{
    std::lock_guard lk(flagMutex);
    flags[flagName] = value;
}
void GameState::notifyObservers(const std::string& flagName, int value)
{
    std::vector<FlagCallback> snapshot;

    {
        std::scoped_lock lk(observersMutex, observersToRemoveMutex);
        removeObserversImpl();

        auto iter = observers.find(flagName);
        if (iter != observers.end())
        {
            for (auto& [id, callback] : iter->second)
            {
                snapshot.push_back(callback);
            }
        }
    }

    for (auto& observer : snapshot)
    {
        observer(flagName, value);
    }
}
void GameState::removeObserversImpl()
{
    for (auto id : observersToRemove)
    {
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
    observersToRemove.clear();
}
}  // namespace GameEngine
