#include "IdPool.h"

#include <algorithm>
#include <mutex>

#include "Logger/Log.h"


namespace Utils
{
IdPool::IdPool(IdType startupValue)
    : counter_{startupValue}
    , startupValue_{startupValue}
{
}
IdType IdPool::getId(const std::optional<IdType>& requestedId)
{
    std::lock_guard<std::mutex> lk(mutex);
    if (requestedId)
    {
        if (counter_ <= *requestedId)
        {
            for (auto i = counter_; i < *requestedId; ++i)
            {
                freeIds_.push_back(i);
            }
            counter_ = *requestedId + 1;
            return *requestedId;
        }
        else
        {
            auto iter = std::find(freeIds_.begin(), freeIds_.end(), *requestedId);

            if (iter != freeIds_.end())
            {
                auto toReturn = *iter;
                freeIds_.erase(iter);
                return toReturn;
            }
            else
            {
                auto toReturn = getNextId();
                LOG_ERROR << "Can not allocate id : " << *requestedId << " assign new one : " << toReturn;
                return toReturn;
            }
        }
    }

    if (freeIds_.empty())
    {
        return getNextId();
    }

    auto toReturn = freeIds_.front();
    freeIds_.pop_front();
    return toReturn;
}

IdType IdPool::getNextId()
{
    return counter_++;
}

void IdPool::releaseId(IdType id)
{
    std::lock_guard<std::mutex> lk(mutex);
    freeIds_.push_back(id);
    freeIds_.sort();
}
void IdPool::clear()
{
    clear(startupValue_);
}
void IdPool::clear(IdType value)
{
    std::lock_guard<std::mutex> lk(mutex);
    counter_ = value;
    freeIds_.clear();
}
}  // namespace Utils
