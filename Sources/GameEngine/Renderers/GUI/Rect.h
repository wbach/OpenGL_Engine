#pragma once
#include "Types.h"

namespace GameEngine
{
struct Rect
{
    vec2ui position;
    vec2ui size;

    Rect(){}
    Rect(const vec2ui& p, const vec2ui& size)
        : position(p)
        , size(size)
    {}
    Rect(uint32 x, uint32 y, uint32 w, uint32 h)
        : position(x, y)
        , size(w, h)
    {

    }
};
}  // namespace GameEngine
