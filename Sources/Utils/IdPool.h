#pragma once
#include <list>
#include <optional>
#include "Types.h"

namespace Utils
{
class IdPool
{
public:
    IdPool();
    IdType getId(const std::optional<IdType>& = std::nullopt);
    void releaseId(IdType);
    void clear();

private:
    IdType getNextId();

private:
    IdType counter_;
    std::list<IdType> freeIds_;
};
}  // namespace Utils
