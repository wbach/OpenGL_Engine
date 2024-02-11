#include "AnimationClipNames.h"

namespace GameEngine
{
namespace Components
{

bool operator==(const MovmentClipNames &l, const MovmentClipNames &r)
{
    return l.forward == r.forward and l.backward == r.backward and l.moveleft == r.moveleft and
           l.moveRight == r.moveRight;
}

bool operator!=(const MovmentClipNames & l , const MovmentClipNames & r)
{
    return not (l == r);
}

}  // namespace Components
}  // namespace GameEngine
