#pragma once
#include <ostream>

namespace GraphicsApi
{
struct DisplayMode
{
    int w;
    int h;
    int refreshRate;
    int displayIndex;
};
inline std::ostream& operator<<(std::ostream& os, const GraphicsApi::DisplayMode& dm)
{
    os << "DisplayMode[" << dm.displayIndex << "]: " << dm.w << "x" << dm.h << " @ " << dm.refreshRate << "Hz";
    return os;
}
}  // namespace GraphicsApi