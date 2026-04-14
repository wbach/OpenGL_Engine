#include "Transform.h"

#include <Logger/Log.h>

namespace GameEngine
{
namespace GUI
{
std::ostream& operator<<(std::ostream& os, const Transform& t)
{
    os << "Transform(pos: [" << t.position.x << ", " << t.position.y << "], "
       << "scale: [" << t.scale.x << ", " << t.scale.y << "], "
       << "zValue: " << t.zValue << " addr: " << &t << ")";
    return os;
}
}  // namespace GUI
}  // namespace GameEngine