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
    AnimationClip(const std::string&);
    float GetLength() const;
    void AddFrame(const KeyFrame&);
    const std::vector<KeyFrame>& GetFrames() const;
    void SetLength(float l);
    KeyFrame* getFrame(float);
    KeyFrame* getFrame(size_t);

public:
    std::string name;

private:
    float length;
    std::vector<KeyFrame> frames;
};
}  // namespace Animation
}  // namespace GameEngine

namespace std
{
string to_string(const GameEngine::Animation::AnimationClip&);
} // namespace std
