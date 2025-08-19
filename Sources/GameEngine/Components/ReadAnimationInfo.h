#pragma once
#include <GameEngine/Components/Animation/AnimationClipInfo.h>
#include <GameEngine/Resources/File.h>

namespace GameEngine
{
namespace Components
{
struct ReadAnimationInfo
{
    std::string name;
    File file;
    bool playInLoop;
    bool useRootMontion;
    float playSpeed{1.f};
};
}  // namespace Components
}  // namespace GameEngine
