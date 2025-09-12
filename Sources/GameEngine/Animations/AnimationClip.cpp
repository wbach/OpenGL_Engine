#include "AnimationClip.h"

#include <Utils/Container.h>

#include <algorithm>
#include <sstream>
#include <Utils/StreamOperators.h>

namespace GameEngine
{
namespace Animation
{
AnimationClip::AnimationClip()
    : AnimationClip("No name")
{
}

AnimationClip::AnimationClip(const std::string& name, const std::optional<File> & file)
    : length(-std::numeric_limits<float>::max())
    , name(name)
    , file(file)
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
    return Utils::get(frames, [time](const auto& frame) { return compare(frame.timeStamp.value, time.value); });
}

KeyFrame* AnimationClip::getFrame(FrameIndex i)
{
    return i.value < frames.size() ? &frames[i.value] : nullptr;
}

std::optional<uint32> AnimationClip::getFrameId(float time)
{
    return std::nullopt;
}

bool AnimationClip::isFromExternalFile() const
{
    return file.has_value();
}

const std::string& AnimationClip::getName() const
{
    return name;
}

const std::optional<File>& AnimationClip::getFile() const
{
    return file;
}
}  // namespace Animation
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::Animation::AnimationClip& clip)
{
    stringstream ss;
    ss << "Animation clip : " << clip.getName() << '\n';
    for (const auto& frame : clip.GetFrames())
    {
        ss << "frame time : " <<  frame.timeStamp.value << '\n';
        for (auto& transform : frame.transforms)
        {
            ss << "name : " << transform.first << " position : " << transform.second.position
               << " rotation : " << transform.second.rotation << '\n';
        }
        ss << "*********************************************";
    }
    return ss.str();
}
}  // namespace std
