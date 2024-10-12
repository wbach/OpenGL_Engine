#include "CollisionContactInfo.h"

namespace std
{
string to_string(const GameEngine::Physics::CollisionContactInfo & info )
{
    return "Pos1=" + std::to_string(info.pos1) + ", Pos2=" + std::to_string(info.pos2) +
           ", rigidbodyId1=" + std::to_string(info.rigidbodyId1) + ", rigidbodyId2=" + std::to_string(info.rigidbodyId2);
}
}  // namespace std