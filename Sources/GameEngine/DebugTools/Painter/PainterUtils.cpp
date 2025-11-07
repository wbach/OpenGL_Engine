#include "PainterUtils.h"

namespace GameEngine
{
void sort(std::vector<Influance>& influance)
{
    std::sort(influance.begin(), influance.end(),
              [](const Influance& a, const Influance& b)
              { return (a.point.y < b.point.y) || (a.point.y == b.point.y && a.point.x < b.point.x); });
}
}  // namespace GameEngine