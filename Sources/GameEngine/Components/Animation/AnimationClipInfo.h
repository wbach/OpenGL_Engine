#pragma once
#include <functional>
#include <vector>

#include "GameEngine/Animations/AnimationClip.h"
#include "PlayDirection.h"

namespace GameEngine
{
namespace Components
{
struct AnimationClipInfo
{
    struct Subscription
    {
        IdType id;
        std::function<void()> callback;
        float timeStamp;
    };

    enum class PlayType
    {
        loop,
        once
    };

    using UseRootMontion = bool;

    float playSpeed{1.f};
    PlayType playType{PlayType::loop};
    PlayDirection playDirection{PlayDirection::forward};
    UseRootMontion rootMontion{false};
    Animation::AnimationClip clip;

    std::vector<Subscription> subscribers;
};
}  // namespace Components
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::Components::AnimationClipInfo::PlayType&);
GameEngine::Components::AnimationClipInfo::PlayType from_string(const std::string&);
}  // namespace std
