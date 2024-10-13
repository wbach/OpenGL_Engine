#pragma once
#include <Types.h>

#include "PlayStateType.h"

namespace GameEngine
{
namespace Components
{
struct AttackAnimation
{
    std::string name;
    PlayStateType stateType;
};

struct MovmentClipNames
{
    std::string forward;
    std::string backward;
    std::string left;
    std::string right;
};

struct RotateAnimationClips
{
    std::string left;
    std::string right;
};

struct PostureClipNames
{
    RotateAnimationClips rotate;
    std::string idle;
    std::string hurt;
    std::string death;
};

struct Movement
{
    MovmentClipNames walk;
    MovmentClipNames run;
    MovmentClipNames crouch;
};

struct Posture
{
    PostureClipNames crouched;
    PostureClipNames stand;
};

struct StateClipsNames
{
    Movement movement;
    Posture posture;
    MovmentClipNames dodge;
    std::string dodgeDive;

    std::string jump;
    std::string sprint;
    std::string falling;

    std::vector<AttackAnimation> attack;
};

struct AimClips
{
    std::string draw;
    std::string recoil;
    std::string idle;
};

struct AnimationClipsNames
{
    StateClipsNames armed;
    StateClipsNames disarmed;
    AimClips aim;

    std::string equip;
    std::string disarm;
};

bool operator==(const MovmentClipNames&, const MovmentClipNames&);
bool operator!=(const MovmentClipNames&, const MovmentClipNames&);
bool operator==(const RotateAnimationClips&, const RotateAnimationClips&);
bool operator!=(const RotateAnimationClips&, const RotateAnimationClips&);
}  // namespace Components
}  // namespace GameEngine
