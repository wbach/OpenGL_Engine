#pragma once
#include <optional>

#include "KeyFrame.h"

namespace GameEngine
{
namespace Animation
{
struct FrameIndex
{
    size_t value;

    FrameIndex& operator=(size_t b)
    {
        value = b;
        return *this;
    }
    FrameIndex& operator=(FrameIndex b)
    {
        value = b.value;
        return *this;
    }
    bool operator==(const FrameIndex& v) const
    {
        return value == v.value;
    }
    bool operator!=(const FrameIndex& v) const
    {
        return value != v.value;
    }
    bool operator<(const FrameIndex& a)
    {
        return value < a.value;
    }
    bool operator>(const FrameIndex& a)
    {
        return value > a.value;
    }
};
class AnimationClip
{
public:
    enum class PlayType
    {
        loop,
        once
    };

    AnimationClip();
    AnimationClip(const std::string&);
    float GetLength() const;
    KeyFrame& AddFrame(const KeyFrame&);
    const std::vector<KeyFrame>& GetFrames() const;
    void SetLength(float l);
    KeyFrame* getFrame(TimeStamp);
    KeyFrame* getFrame(FrameIndex);
    std::optional<uint32> getFrameId(float);

public:
    std::string name;
    std::string filePath;
    PlayType playType;

private:
    float length;
    std::vector<KeyFrame> frames;
};
}  // namespace Animation
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::Animation::AnimationClip&);
}  // namespace std
