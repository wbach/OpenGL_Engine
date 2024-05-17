#include "AnimationClip.h"
#include <algorithm>
#include <sstream>

namespace GameEngine
{
namespace Animation
{
AnimationClip::AnimationClip()
    : AnimationClip("No name")
{
}

AnimationClip::AnimationClip(const std::string& name)
    : name(name)
    , playType(PlayType::loop)
    , length(-std::numeric_limits<float>::max())
{
}

float AnimationClip::GetLength() const
{
    return length;
}

KeyFrame& AnimationClip::AddFrame(const KeyFrame& frame)
{
    auto it = std::lower_bound(frames.begin(), frames.end(), frame,
                               [](const auto& a, const auto& b) { return a.timeStamp < b.timeStamp; });

    auto insertedFrameIter = frames.insert(it, frame);

    if (frame.timeStamp > length)
        length = frame.timeStamp.value;

    return *insertedFrameIter;
}

const std::vector<KeyFrame>& AnimationClip::GetFrames() const
{
    return frames;
}

void AnimationClip::SetLength(float l)
{
    length = l;
}

KeyFrame* AnimationClip::getFrame(TimeStamp time)
{
    auto iter = std::find_if(frames.begin(), frames.end(),
    [time](const auto& frame) { return compare(frame.timeStamp.value, time.value); });

    return iter != frames.end() ? &*iter : nullptr;
}

KeyFrame* AnimationClip::getFrame(FrameIndex i)
{
    return i.value < frames.size() ? &frames[i.value] : nullptr;
}

std::optional<uint32> AnimationClip::getFrameId(float time)
{

    return std::nullopt;
}

}  // namespace Animation
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::Animation::AnimationClip& clip)
{
    stringstream ss;
    ss << "Animation clip : " << clip.name << '\n';
    for (const auto& frame : clip.GetFrames())
    {
        ss << "frame time : " << std::to_string(frame.timeStamp.value) << '\n';
        for (auto& transform : frame.transforms)
        {
            ss << "name : " << transform.first << " position : " << std::to_string(transform.second.position)
               << " rotation : " + std::to_string(transform.second.rotation) << '\n';
        }
        ss << "*********************************************";
    }
    return ss.str();
}
}  // namespace std
