#pragma once
#include <vector>

#include "Influance.h"

namespace GameEngine
{
class IBrush
{
public:
    virtual ~IBrush() = default;

    virtual void createInfluance(bool, float stepX, float stepZ) = 0;
    virtual const std::vector<Influance>& getInfluence() const   = 0;
};
}  // namespace GameEngine
