#pragma once
#include <Types.h>

namespace GameEngine
{
class IBufferDataUpdaterSubcriber
{
public:
    virtual ~IBufferDataUpdaterSubcriber() = default;
    virtual void Update()                  = 0;
    virtual uint32 GetId() const           = 0;
};
}  // namespace GameEngine