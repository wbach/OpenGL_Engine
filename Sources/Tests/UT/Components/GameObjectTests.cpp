#include <gtest/gtest.h>

#include "Components/Animation/Animator.h"
#include "Components/ComponentsReadFunctions.h"
#include "Components/Renderer/Entity/RendererComponent.hpp"
#include "Objects/GameObject.h"
#include "Tests/UT/Components/BaseComponent.h"
#include "TreeNode.h"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;

namespace
{
constexpr char model[] = "testModel.obj";
constexpr char clip[]  = "testClip";
}  // namespace

struct GameObjectTestSchould : public BaseComponentTestSchould
{
    GameObjectTestSchould()
        : sut_(obj_)
    {
    }
    void SetUp() override
    {
    }

    void verifyComponents()
    {
        auto rendererComponent = sut_.GetComponent<RendererComponent>();
        auto animator          = sut_.GetComponent<Animator>();

        EXPECT_TRUE(animator != nullptr);
        EXPECT_TRUE(rendererComponent != nullptr);

        EXPECT_EQ(model, rendererComponent->fileName_LOD1);
        EXPECT_NE(std::find_if(animator->animationClips.begin(), animator->animationClips.end(),
                               [&expectedClip = clip](const auto& clip) { return clip.name == expectedClip; }),
                  animator->animationClips.end());
    }

    GameObject& sut_;
};

TEST_F(GameObjectTestSchould, AddAndGetComponentByTemplete)
{
    sut_.AddComponent<RendererComponent>().AddModel(model);
    sut_.AddComponent<Animator>().AddAnimationClip(clip, "testFile");

    verifyComponents();
}

TEST_F(GameObjectTestSchould, AddByTreeNodeAndGetComponentByTemplete)
{
    Animator::registerReadFunctions();
    RendererComponent::registerReadFunctions();

    {
        TreeNode node("Component");
        node.attributes_["type"]              = "Animator";
        auto& animationClipsNode              = node.addChild("animationClips");
        auto& animationClipNode               = animationClipsNode.addChild("animationClip");
        animationClipNode.attributes_["name"] = clip;
        sut_.AddComponent(node);
    }

    {
        TreeNode node("Component");
        node.attributes_["type"] = "RendererComponent";
        auto& modelFileNamesNode = node.addChild("modelFileNames");
        {
            auto& modelFileNameNode = modelFileNamesNode.addChild("modelFileName");
            modelFileNameNode.addChild("fileName", std::string(model));
            modelFileNameNode.addChild("lvlOfDetail", std::string("0"));
        }
        sut_.AddComponent(node);
    }

    verifyComponents();

    unregsiterComponentReadFunction("Animator");
    unregsiterComponentReadFunction("RendererComponent");

    EXPECT_TRUE(ReadFunctions::instance().getComponentTypeNameToId().empty());
}