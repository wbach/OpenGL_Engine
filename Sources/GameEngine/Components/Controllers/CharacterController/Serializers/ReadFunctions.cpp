#include "ReadFunctions.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "../AnimationClipNames.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "Logger/Log.h"
#include "Variables.h"

using namespace GameEngine::Components;
using namespace GameEngine::Components::Character;

void Read(const TreeNode& node, MovmentClipNames& result)
{
    Read(node.getChild(CSTR_FORWARD), result.forward);
    Read(node.getChild(CSTR_BACKWARD), result.backward);
    Read(node.getChild(CSTR_LEFT), result.left);
    Read(node.getChild(CSTR_RIGHT), result.right);
}

void Read(const TreeNode& node, GameEngine::AnimSequence& result)
{
    if (not node.value_.empty())
    {
        result.clipNames.push_back(node.value_);
    }
    else
    {
        if (auto sequnece = node.getChild(CSTR_SEQUENCE))
        {
            for (const auto& child : sequnece->getChildren())
            {
                result.clipNames.push_back(child->value_);
            }
        }
    }
}

void Read(const TreeNode& node, std::vector<AttackAnimation>& result)
{
    for (const auto& node : node.getChildren())
    {
        if (not node->value_.empty())
        {
            result.push_back(AttackAnimation{.clipsSequence = GameEngine::AnimSequence{.clipNames = {node->value_}},
                                             .stateType     = PlayStateType::idle});
        }
        else
        {
            AttackAnimation aa{{}, PlayStateType::idle};
            auto maybeNameNode = node->getChild(CSTR_CLIP_NAME);
            if (maybeNameNode)
            {
                std::string name;
                Read(*maybeNameNode, aa.clipsSequence);
            }
            else
            {
                LOG_DEBUG << "clipname node not exist";
            }

            auto maybePlayStateTypeNode = node->getChild(CSTR_ANIMATION_STATE_TYPE);
            if (maybePlayStateTypeNode)
            {
                if (maybePlayStateTypeNode->value_ == "walk")
                {
                    aa.stateType = PlayStateType::walk;
                }
                if (maybePlayStateTypeNode->value_ == "run")
                {
                    aa.stateType = PlayStateType::run;
                }
            }

            result.push_back(aa);
        }
    }
}
void Read(const TreeNode& node, RotateAnimationClips& result)
{
    Read(node.getChild(CSTR_LEFT), result.left);
    Read(node.getChild(CSTR_RIGHT), result.right);
}

void Read(const TreeNode& node, PostureClipNames& result)
{
    Read(node.getChild(CSTR_IDLE_MAIN), result.idle);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_IDLE), result.idle);
    Read(node.getChild(CSTR_HURT_ANIMATION), result.hurt);
    Read(node.getChild(CSTR_DEATH_ANIMATION), result.death);
    Read(node.getChild(CSTR_ROTATE), result.rotate);
}

void Read(const TreeNode& node, Posture& result)
{
    Read(node.getChild(CSTR_POSTURE_STAND), result.stand);
    Read(node.getChild(CSTR_POSTURE_CROUCHED), result.crouched);
}

void Read(const TreeNode& node, Movement& result)
{
    Read(node.getChild(CSTR_ANIMATION_WALK), result.walk);
    Read(node.getChild(CSTR_ANIMATION_RUN), result.run);
    Read(node.getChild(CSTR_ANIMATION_CROUCH_MOVEMENT), result.crouch);
}

void Read(const TreeNode& node, DialogAnimations& result)
{
    Read(node.getChild(CSTR_DIALOG_SAY), result.say);
    Read(node.getChild(CSTR_DIALOG_LISTEN), result.listen);
}

void Read(const TreeNode& node, StateClipsNames& result)
{
    Read(node.getChild(CSTR_MOVEMENT), result.movement);
    Read(node.getChild(CSTR_POSTURE), result.posture);
    Read(node.getChild(CSTR_DIALOG), result.dialog);
    Read(node.getChild(CSTR_JUMP_ANIMATION), result.jump);
    Read(node.getChild(CSTR_ATTACK_ANIMATIONS), result.attack);
    Read(node.getChild(CSTR_ANIMATION_SPRINT), result.sprint);
    Read(node.getChild(CSTR_ANIMATION_DODGE_DIVE), result.dodgeDive);
    Read(node.getChild(CSTR_ANIMATION_DODGE), result.dodge);
    Read(node.getChild(CSTR_FALLINING_IDLE_ANIMATION), result.falling);
}

void Read(const TreeNode& node, AimClips& result)
{
    Read(node.getChild(CSTR_DRAW_ARROW_ANIMATION), result.draw);
    Read(node.getChild(CSTR_RECOIL_ARROW_ANIMATION), result.recoil);
    Read(node.getChild(CSTR_AIM_IDLE_ANIMATION), result.idle);
}

void Read(const TreeNode& node, AnimationClipsNames& result)
{
    Read(node.getChild(CSTR_ANIMATION_ARMED), result.armed);
    Read(node.getChild(CSTR_ANIMATION_DISARMED), result.disarmed);

    Read(node.getChild(CSTR_ANIMATION_EQUIP), result.equip);
    Read(node.getChild(CSTR_ANIMATION_DISARM), result.disarm);

    Read(node.getChild(CSTR_AIM), result.aim);
}

namespace GameEngine::Components::Character
{
void Read(const TreeNode* node, AnimationClipsNames& v)
{
    if (node)
    {
        ::Read(*node, v);
    }
}
}  // namespace GameEngine::Components::Character
