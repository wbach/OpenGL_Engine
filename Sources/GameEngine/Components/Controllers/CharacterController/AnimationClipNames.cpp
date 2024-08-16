#include "AnimationClipNames.h"

namespace GameEngine
{
namespace Components
{

bool operator==(const MovmentClipNames &l, const MovmentClipNames &r)
{
    return l.forward == r.forward and l.backward == r.backward and l.left == r.left and l.right == r.right;
}

bool operator!=(const MovmentClipNames &l, const MovmentClipNames &r)
{
    return not(l == r);
}
bool operator==(const RotateAnimationClips &l, const RotateAnimationClips &r)
{
    return l.left == r.left and l.right == r.right;
}
bool operator!=(const RotateAnimationClips &l, const RotateAnimationClips &r)
{
    return not(l == r);
}
}  // namespace Components
}  // namespace GameEngine
