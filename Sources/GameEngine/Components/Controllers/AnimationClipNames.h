#pragma once
#include <string>
#include <vector>

namespace GameEngine
{
namespace Components
{
struct MovmentClipNames
{
    std::string forward;
    std::string backward;
    std::string rotateLeft;
    std::string rotateRight;
    std::string moveleft;
    std::string moveRight;
};

struct StateClipsNames
{
    MovmentClipNames walk;
    MovmentClipNames run;
    MovmentClipNames crouch;
    std::string jump;
    std::string idle;
    std::string hurt;
    std::string death;

    std::vector<std::string> attack;
};

struct AnimationClipsNames
{
    StateClipsNames armed;
    StateClipsNames disarmed;

    std::string sprint;
    std::string doge;
    std::string dogeJump;
    std::string equip;
    std::string disarm;
};
}  // namespace Components
}  // namespace GameEngine
