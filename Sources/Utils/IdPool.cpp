#include "IdPool.h"

#include <limits>
#include <stdexcept>
#include <thread>

#include "Logger/Log.h"

namespace Utils
{
IdPool::IdPool(IdType startupValue, IdReusePolicy reusePolicy)
    : counter_{startupValue}
    , startupValue_{startupValue}
    , reusePolicy_{reusePolicy}
{
}

IdType IdPool::getId(const std::optional<IdType>& requestedId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto result = getIdUnlocked(requestedId);
    return result;
}

void IdPool::releaseId(IdType id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    releaseIdUnlocked(id);
}

void IdPool::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    clearUnlocked();
}

void IdPool::clear(IdType value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    clearUnlocked(value);
}

IdType IdPool::getIdUnlocked(const std::optional<IdType>& requestedId)
{
    if (requestedId)
    {
        return getRequestedId(*requestedId);
    }

    return getNextAvailableId();
}

void IdPool::releaseIdUnlocked(IdType id)
{
    if (isReleased(id))
    {
        return;
    }

    switch (reusePolicy_)
    {
        case IdReusePolicy::Immediate:
            freeIds_.insert(id);
            break;

        case IdReusePolicy::OnExhaustion:
        case IdReusePolicy::Delayed:
            deferredIds_.insert(id);
            break;
    }
}

void IdPool::clearUnlocked()
{
    clearUnlocked(startupValue_);
}

void IdPool::clearUnlocked(IdType value)
{
    counter_          = value;
    counterExhausted_ = false;

    freeIds_.clear();
    deferredIds_.clear();
}

IdType IdPool::getRequestedId(IdType id)
{
    if (removeId(freeIds_, id) || removeId(deferredIds_, id))
    {
        return id;
    }

    if (!counterExhausted_ && counter_ <= id)
    {
        return allocateRequestedId(id);
    }

    auto assignedId = getNextAvailableId();

    // LOG_ERROR << "Can not allocate id : " << id
    //           << " assign new one : " << assignedId;

    return assignedId;
}

IdType IdPool::getNextAvailableId()
{
    if (!freeIds_.empty())
    {
        auto iter = freeIds_.begin();
        auto id   = *iter;
        freeIds_.erase(iter);
        return id;
    }

    if (!counterExhausted_)
    {
        return getNextId();
    }

    if (!deferredIds_.empty())
    {
        auto iter = deferredIds_.begin();
        auto id   = *iter;
        deferredIds_.erase(iter);
        return id;
    }

    throw std::runtime_error("IdPool exhausted");
}

IdType IdPool::getNextId()
{
    if (counterExhausted_)
    {
        throw std::runtime_error("IdPool exhausted");
    }

    auto id = counter_;

    if (counter_ == std::numeric_limits<IdType>::max())
    {
        counterExhausted_ = true;
    }
    else
    {
        ++counter_;
    }

    return id;
}

IdType IdPool::allocateRequestedId(IdType id)
{
    for (auto i = counter_; i < id; ++i)
    {
        freeIds_.insert(i);
    }

    if (id == std::numeric_limits<IdType>::max())
    {
        counterExhausted_ = true;
    }
    else
    {
        counter_ = id + 1;
    }

    return id;
}

bool IdPool::removeId(std::set<IdType>& ids, IdType id)
{
    return ids.erase(id) > 0;
}

bool IdPool::isReleased(IdType id) const
{
    return freeIds_.find(id) != freeIds_.end() || deferredIds_.find(id) != deferredIds_.end();
}

}  // namespace Utils
