#pragma once
#include <Types.h>

#include <vector>

namespace GameEngine
{
class INavigationProvider
{
public:
    virtual ~INavigationProvider()                                    = default;
    virtual std::vector<vec3> CalculatePath(const vec3&, const vec3&) = 0;
    virtual bool IsWalkable(const vec3&)                              = 0;
};
}  // namespace GameEngine
