#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Components
{
struct AttackAnimation
{
    enum class PlayStateType
    {
        idle,
        walk,
        run
    };
    std::string name;
    PlayStateType stateType;
};

struct MovmentClipNames
{
    std::string forward;
    std::string backward;
    std::string moveleft;
    std::string moveRight;
};

struct StateClipsNames
{
    MovmentClipNames walk;
    MovmentClipNames run;
    MovmentClipNames crouch;

    std::string rotateLeft;
    std::string rotateRight;
    std::string jump;
    std::string idle;
    std::string hurt;
    std::string death;
    std::string sprint;

    std::vector<AttackAnimation> attack;
};

struct AnimationClipsNames
{
    StateClipsNames armed;
    StateClipsNames disarmed;

    std::string doge;
    std::string dogeJump;
    std::string equip;
    std::string disarm;
    std::string drawArrow;
    std::string recoilArrow;
    std::string aimIdle;
};

bool operator==(const MovmentClipNames&, const MovmentClipNames&);
bool operator!=(const MovmentClipNames&, const MovmentClipNames&);
}  // namespace Components
}  // namespace GameEngine
