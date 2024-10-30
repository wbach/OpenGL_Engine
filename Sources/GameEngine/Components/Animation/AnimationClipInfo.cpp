#include "AnimationClipInfo.h"
#include <Utils/Utils.h>

namespace std
{
string to_string(const GameEngine::Components::AnimationClipInfo::PlayType& type)
{
    if (type == GameEngine::Components::AnimationClipInfo::PlayType::loop)
    {
        return "loop";
    }
    else if (type == GameEngine::Components::AnimationClipInfo::PlayType::once)
    {
        return "once";
    }
    return "unkown";
}
GameEngine::Components::AnimationClipInfo::PlayType from_string(const std::string& inputStr)
{
    if (not inputStr.empty())
    {
        if (inputStr == Utils::toLower("loop"))
        {
            return GameEngine::Components::AnimationClipInfo::PlayType::loop;
        }
        else if (inputStr == Utils::toLower("once"))
        {
            return GameEngine::Components::AnimationClipInfo::PlayType::once;
        }
    }
    return GameEngine::Components::AnimationClipInfo::PlayType::loop;
}
}  // namespace std
