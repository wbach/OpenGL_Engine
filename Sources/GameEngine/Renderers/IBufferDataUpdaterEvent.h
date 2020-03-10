#pragma once

namespace GameEngine
{
class IBufferDataUpdaterEvent
{
public:
    virtual ~IBufferDataUpdaterEvent() = default;
    virtual void Execute()             = 0;
};
}  // namespace GameEngine