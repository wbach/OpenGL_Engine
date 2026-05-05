#pragma once
#include <Types.h>

#include "PlayStateType.h"

namespace GameEngine
{
struct AnimSequence
{
    std::vector<std::string> clipNames;

    auto operator<=>(const AnimSequence&) const = default;

    auto operator=(const AnimSequence&) -> AnimSequence& = default;
    auto operator=(const std::vector<std::string>& names) -> AnimSequence&
    {
        clipNames = names;
        return *this;
    }

    bool empty() const
    {
        return clipNames.empty();
    }

    void clear()
    {
        clipNames.clear();
    }

    size_t size() const
    {
        return clipNames.size();
    }

    const std::string& front() const
    {
        return clipNames.front();
    }
    const std::string& back() const
    {
        return clipNames.back();
    }

    const std::string& operator[](size_t index) const
    {
        return clipNames[index];
    }

    std::string& operator[](size_t index)
    {
        return clipNames[index];
    }

    auto begin() { return clipNames.begin(); }
    auto end()   { return clipNames.end(); }

    auto begin() const { return clipNames.begin(); }
    auto end()   const { return clipNames.end(); }
};

namespace Components
{
struct AttackAnimation
{
    AnimSequence clipsSequence;
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
struct DialogAnimations
{
    std::string listen;
    std::string say;
};

struct StateClipsNames
{
    Movement movement;
    Posture posture;
    MovmentClipNames dodge;
    DialogAnimations dialog;
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

    AnimSequence equip;
    AnimSequence disarm;
};

bool operator==(const MovmentClipNames&, const MovmentClipNames&);
bool operator!=(const MovmentClipNames&, const MovmentClipNames&);
bool operator==(const RotateAnimationClips&, const RotateAnimationClips&);
bool operator!=(const RotateAnimationClips&, const RotateAnimationClips&);
}  // namespace Components
}  // namespace GameEngine
