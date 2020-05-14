#pragma once
#include <Types.h>

namespace GraphicsApi
{
class GraphicsObject
{
public:
    GraphicsObject();
    uint64 GetId() const;

private:
    uint64 id;
    static uint64 s_id;
};
}  // namespace GraphicsApi
