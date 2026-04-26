#include "AnimationClipInfo.h"

#include <Utils/Utils.h>

#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
std::ostream& operator<<(std::ostream& os, const AnimationClipInfo& info)
{
    os << "AnimationClipInfo {\n"
       << "  playSpeed: " << info.playSpeed << "\n"
       << "  playType: " << magic_enum::enum_name(info.playType) << "\n"
       << "  playDirection: " << magic_enum::enum_name(info.playDirection) << "\n"
       << "  rootMotion: " << Utils::BoolToString(info.rootMontion) << "\n";

    os << "  clip: " << info.clip.getName() << "\n";

    os << "  subscribers: [" << info.subscribers.size() << "]";

    if (not info.subscribers.empty())
    {
        os << " { ";
        for (const auto& sub : info.subscribers)
        {
            os << "{ id: " << sub.id << ", time: " << sub.timeStamp << " } ";
        }
        os << "}";
    }

    os << "\n}";
    return os;
}
}  // namespace Components
}  // namespace GameEngine

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
std::optional<GameEngine::Components::AnimationClipInfo::PlayType> from_string(const std::string& inputStr)
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
    return std::nullopt;
}
}  // namespace std
