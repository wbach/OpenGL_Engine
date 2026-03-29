#pragma once
#include <EngineApi.h>
#include <IdPool.h>
#include <Types.h>

#include <functional>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
namespace GameEngine
{
class ENGINE_API GameState
{
public:
    using FlagCallback = std::function<void(const std::string&, int)>;

    bool hasFlag(const std::string&) const;
    std::optional<int> getFlag(const std::string&) const;
    void setFlag(const std::string&, int value);
    IdType addObserver(const std::string&, FlagCallback);
    void removeObserver(IdType);

private:
    void setFlagImpl(const std::string&, int value);
    void removeObserversImpl();
    void notifyObservers(const std::string&, int);

private:
    std::unordered_map<std::string, std::unordered_map<IdType, FlagCallback>> observers;
    std::unordered_map<IdType, std::string> observersIds;
    std::unordered_map<std::string, int> flags;
    Utils::IdPool observerIdPool;
    std::set<IdType> observersToRemove;

    mutable std::mutex flagMutex;
    mutable std::mutex observersMutex;
    mutable std::mutex observersToRemoveMutex;
};
}  // namespace GameEngine
