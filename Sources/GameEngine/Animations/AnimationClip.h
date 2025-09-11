#pragma once
#include <optional>

#include "KeyFrame.h"
#include "GameEngine/Resources/File.h"

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
    AnimationClip();
    AnimationClip(const std::string&, const std::optional<File>& = std::nullopt);
    float GetLength() const;
    KeyFrame& AddFrame(const KeyFrame&);
    const std::vector<KeyFrame>& GetFrames() const;
    void SetLength(float l);
    KeyFrame* getFrame(TimeStamp);
    KeyFrame* getFrame(FrameIndex);
    std::optional<uint32> getFrameId(float);
    bool isFromExternalFile() const;
    const std::string& getName() const;
    const std::optional<File>& getFile() const;

private:
    float length;
    std::string name;
    std::vector<KeyFrame> frames;
    std::optional<File> file;
};
}  // namespace Animation
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::Animation::AnimationClip&);
}  // namespace std
