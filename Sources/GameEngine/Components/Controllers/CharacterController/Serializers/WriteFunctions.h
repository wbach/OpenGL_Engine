#pragma once
class TreeNode;

namespace GameEngine::Components
{
struct AnimationClipsNames;

namespace Character
{
void write(TreeNode&, const AnimationClipsNames&);
}
}  // namespace GameEngine::Components
