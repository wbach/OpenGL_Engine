#include <Dialogs/GameState.h>
#include <GameEngine/Dialogs/DialogueManager.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Menu/Menu.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/TreeView/TreeView.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Renderers/GUI/GuiManager.h>
#include <Scene/TweenManager.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>

#include "Components/ComponentController.h"
#include "Components/ComponentFactory.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "IdPool.h"
#include "Image/Image.h"
#include "Objects/GameObject.h"
#include "Tests/Mocks/Api/InputManagerMock.h"
#include "Tests/Mocks/Gui/GuiElementFactoryMock.h"
#include "Tests/Mocks/Resources/TextureLoaderMock.h"
#include "Tests/Mocks/Utils/TimerServiceMock.h"
#include "Tests/UT/Components/BaseComponent.h"
#include "gmock/gmock.h"

using namespace std::chrono_literals;
using namespace testing;
using namespace GameEngine;

class DialogueManagerTests : public BaseComponentTestSchould
{
protected:
    DialogueManagerTests()
        : sut(timerService_, inputManagerMock_, guiElementFactory_, guiManager_, gameState_, tweenManager)
        , texture(graphicsApiMock_, Utils::Image{}, {}, {})
    {
        EXPECT_CALL(resourcesManager_, GetTextureLoader()).WillRepeatedly(ReturnRef(textureLoaderMock));
        EXPECT_CALL(textureLoaderMock, CreateTexture(_, _, Matcher<Utils::Image&&>(_))).WillRepeatedly(Return(&texture));
        dialogueComponent = &obj_->AddComponent<Components::DialogueComponent>();
    }

    virtual void SetUp() override
    {
    }

    void demo()
    {
        Components::DialogueComponent::Nodes nodes;
        nodes[0] = {0, 0, "Stoj! Kto idzie?", {{"Szukam schronienia.", 2, ""}, {"Nie Twoj interes.", -1, ""}}};
        nodes[1] = {
            1, 0, "O co chodzi?.", {{"Co to za oboz?", 2, ""}, {"Powiedz mi wiecej o obozie.", 3, ""}, {"A nic.", -1, ""}}};
        nodes[2] = {2,
                    0,
                    "To Oboz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?",
                    {{"Tak, opowiedz mi.", 3, ""}, {"Nie, musze leciec.", -1, ""}}};
        nodes[3] = {
            3, 0, "Duzo by gadac..., ale wydajesz sie porzadny pogadamy w srodku.", {{"Dziekuje!", -2, "invited_to_camp"}}};
        nodes[4] = {4, 0, "Do zobaczenia w srodku.", {{"Do zobaczenia", -2, "invited_to_camp"}}};
        dialogueComponent->setNodes(std::move(nodes));
    }

    TextureLoaderMock textureLoaderMock;
    DialogueManager sut;
    GeneralTexture texture;
    Components::DialogueComponent* dialogueComponent;
};

TEST_F(DialogueManagerTests, ShouldNotStartDialogueWhenComponentIsEmpty)
{
    dialogueComponent->setNodes({});
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    sut.startDialogue(*obj_, *dialogueComponent);

    EXPECT_FALSE(sut.isActive());
}

TEST_F(DialogueManagerTests, ShouldAutomaticallyGoToNextNodeWhenNoOptionsAvailable)
{
    int currentId = 0;
    int nextId    = 1;

    DialogueNode node;
    node.id         = currentId;
    node.nextNodeID = nextId;
    node.npcText    = "Test text";
    node.options    = {};

    DialogueNode node2;
    node.id         = nextId;
    node.nextNodeID = -1;
    node.npcText    = "Test text";
    node.options    = {};

    Components::DialogueComponent::Nodes nodes{{currentId, node}, {nextId, node2}};
    dialogueComponent->setNodes(std::move(nodes));
    dialogueComponent->startNodeID = currentId;
    dialogueComponent->resetCurrent();

    std::function<void()> timerCallback;
    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&timerCallback), Return(1)));

    sut.startDialogue(*obj_, *dialogueComponent);
    timerCallback();

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 1);
}

TEST_F(DialogueManagerTests, ShouldTransitionToCorrectNodeWhenOptionIsSelected)
{
    demo();
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstTimer;
    std::function<void()> secondTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&secondTimer), Return(2)));

    sut.startDialogue(*obj_, *dialogueComponent);

    firstTimer();

    sut.selectOption(0);

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 2);
    EXPECT_EQ(dialogueComponent->getCurrent()->npcText, "To Oboz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?");
}
