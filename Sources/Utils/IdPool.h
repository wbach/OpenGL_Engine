#pragma once
#include <list>
#include <optional>
#include "Types.h"

namespace Utils
{
class IdPool
{
public:
    IdPool(IdType startupValue = 0);
    IdType getId(const std::optional<IdType>& = std::nullopt);
    void releaseId(IdType);
    void clear();
    void clear(IdType);

private:
    IdType getNextId();

private:
    IdType counter_;
    IdType startupValue_;
    std::list<IdType> freeIds_;

};
}  // namespace Utils
