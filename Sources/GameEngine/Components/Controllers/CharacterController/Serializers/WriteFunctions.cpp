#include "WriteFunctions.h"

#include <Utils/TreeNode.h>
#include <Utils/XML/XmlWriter.h>

#include "../AnimationClipNames.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "Variables.h"

namespace GameEngine::Components::Character
{
void write(TreeNode& node, const MovmentClipNames& names)
{
    write(node.addChild(CSTR_FORWARD), names.forward);
    write(node.addChild(CSTR_BACKWARD), names.backward);
    write(node.addChild(CSTR_LEFT), names.left);
    write(node.addChild(CSTR_RIGHT), names.right);
}

void write(TreeNode& node, const PlayStateType& stateType)
{
    std::string s{"unknown"};
    switch (stateType)
    {
        case PlayStateType::idle:
            s = "idle";
            break;
        case PlayStateType::walk:
            s = "walk";
            break;
        case PlayStateType::run:
            s = "run";
            break;
    }
    write(node, s);
}

void write(TreeNode& node, const std::vector<AttackAnimation>& names)
{
    for (const auto& anim : names)
    {
        auto& n = node.addChild(CSTR_ANIMATION);

        write(n.addChild(CSTR_NAME), anim.name);
        write(n.addChild(CSTR_ANIMATION_STATE_TYPE), anim.stateType);
    }
}

void write(TreeNode& node, const Movement& movement)
{
    write(node.addChild(CSTR_ANIMATION_WALK), movement.walk);
    write(node.addChild(CSTR_ANIMATION_RUN), movement.run);
    write(node.addChild(CSTR_ANIMATION_CROUCH_MOVEMENT), movement.crouch);
}

void write(TreeNode& node, const RotateAnimationClips& rotateClips)
{
    write(node.addChild(CSTR_LEFT), rotateClips.left);
    write(node.addChild(CSTR_RIGHT), rotateClips.right);
}

void write(TreeNode& node, const PostureClipNames& postureClips)
{
    write(node.addChild(CSTR_IDLE_MAIN), postureClips.idle);
    write(node.addChild(CSTR_DEATH_ANIMATION), postureClips.death);
    write(node.addChild(CSTR_HURT_ANIMATION), postureClips.hurt);
    write(node.addChild(CSTR_ROTATE), postureClips.rotate);
}

void write(TreeNode& node, const Posture& posture)
{
    write(node.addChild(CSTR_POSTURE_STAND), posture.stand);
    write(node.addChild(CSTR_POSTURE_CROUCHED), posture.crouched);
}

void write(TreeNode& node, const StateClipsNames& names)
{
    write(node.addChild(CSTR_MOVEMENT), names.movement);
    write(node.addChild(CSTR_POSTURE), names.posture);
    write(node.addChild(CSTR_JUMP_ANIMATION), names.jump);
    write(node.addChild(CSTR_ATTACK_ANIMATIONS), names.attack);
    write(node.addChild(CSTR_ANIMATION_SPRINT), names.sprint);
    write(node.addChild(CSTR_ANIMATION_DODGE), names.dodge);
    write(node.addChild(CSTR_ANIMATION_DODGE_DIVE), names.dodgeDive);
    write(node.addChild(CSTR_FALLINING_IDLE_ANIMATION), names.falling);
}

void write(TreeNode& node, const AimClips& names)
{
    write(node.addChild(CSTR_DRAW_ARROW_ANIMATION), names.draw);
    write(node.addChild(CSTR_RECOIL_ARROW_ANIMATION), names.recoil);
    write(node.addChild(CSTR_AIM_IDLE_ANIMATION), names.idle);
}

void write(TreeNode& node, const AnimationClipsNames& names)
{
    write(node.addChild(CSTR_ANIMATION_ARMED), names.armed);
    write(node.addChild(CSTR_ANIMATION_DISARMED), names.disarmed);

    write(node.addChild(CSTR_ANIMATION_EQUIP), names.equip);
    write(node.addChild(CSTR_ANIMATION_DISARM), names.disarm);
    write(node.addChild(CSTR_AIM), names.aim);
}
}  // namespace GameEngine::Components::Character
