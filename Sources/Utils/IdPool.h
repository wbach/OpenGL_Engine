#pragma once

#include <mutex>
#include <optional>
#include <set>

#include "EngineApi.h"
#include "Types.h"

namespace Utils
{
class ENGINE_API IdPool
{
public:
    enum class IdReusePolicy
    {
        Immediate,
        OnExhaustion,
        Delayed
    };

    IdPool(IdType startupValue = 0, IdReusePolicy reusePolicy = IdReusePolicy::Immediate);
    virtual ~IdPool() = default;

    IdType getId(const std::optional<IdType>& requestedId = std::nullopt);
    void releaseId(IdType id);

    void clear();
    void clear(IdType value);

protected:
    IdType getIdUnlocked(const std::optional<IdType>& requestedId = std::nullopt);
    void releaseIdUnlocked(IdType id);

    void clearUnlocked();
    void clearUnlocked(IdType value);

private:
    IdType getRequestedId(IdType id);
    IdType getNextAvailableId();
    IdType getNextId();
    IdType allocateRequestedId(IdType id);

    bool removeId(std::set<IdType>& ids, IdType id);
    bool isReleased(IdType id) const;

private:
    IdType counter_;
    IdType startupValue_;
    IdReusePolicy reusePolicy_;

    bool counterExhausted_{false};

    std::set<IdType> freeIds_;
    std::set<IdType> deferredIds_;

    std::mutex mutex_;
};
}  // namespace Utils
