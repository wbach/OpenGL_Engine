#pragma once
class TreeNode;

namespace GameEngine::Components
{
struct AnimationClipsNames;

namespace Character
{
void Read(const TreeNode*, AnimationClipsNames&);
}
}  // namespace GameEngine::Components
