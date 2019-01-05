#pragma once
#include "KeyFrame.h"

namespace GameEngine
{
namespace Animation
{
class AnimationClip
{
public:
    AnimationClip();
    AnimationClip(const std::string& name);
    inline float GetLength() const;
    inline void AddFrame(const KeyFrame& frame);
    inline const std::vector<KeyFrame>& GetFrames() const;
    inline void SetLength(float l);

public:
    std::string name;

private:
    float length;
    std::vector<KeyFrame> frames;
};

inline AnimationClip::AnimationClip()
    : AnimationClip("No name")
{
}

inline AnimationClip::AnimationClip(const std::string& name)
    : name(name)
    , length(-std::numeric_limits<float>::max())
{
}

float AnimationClip::GetLength() const
{
    return length;
}

void AnimationClip::AddFrame(const KeyFrame& frame)
{
    frames.push_back(frame);
    if (frame.timeStamp > length)
        length = frame.timeStamp;
}

inline const std::vector<KeyFrame>& AnimationClip::GetFrames() const
{
    return frames;
}

inline void AnimationClip::SetLength(float l)
{
    length = l;
}

}  // namespace Animation
}  // namespace GameEngine
