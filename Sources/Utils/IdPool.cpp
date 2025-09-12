#include "IdPool.h"

#include <algorithm>

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
                /* LOG TO FIX*/  LOG_ERROR << ("Can not allocate id : " + std::to_string(*requestedId) +
                            " assign new one : " + std::to_string(toReturn));
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
    freeIds_.push_back(id);
    freeIds_.sort();
}
void IdPool::clear()
{
    clear(startupValue_);
}
void IdPool::clear(IdType value)
{
    counter_ = value;
    freeIds_.clear();
}
}  // namespace Utils
