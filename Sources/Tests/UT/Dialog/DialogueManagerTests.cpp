#include <Dialogs/GameState.h>
#include <GameEngine/Dialogs/DialogueManager.h>
#include <Renderers/GUI/GuiManager.h>
#include <Scene/TweenManager.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <memory>
#include <string>

#include "Components/ComponentController.h"
#include "Components/ComponentFactory.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "IdPool.h"
#include "Image/Image.h"
#include "Logger/Log.h"
#include "Objects/GameObject.h"
#include "Renderers/GUI/GuiRenderer.h"
#include "Renderers/GUI/Layout/VerticalLayout.h"
#include "Renderers/GUI/Text/GuiTextElement.h"
#include "Renderers/GUI/Window/GuiWindow.h"
#include "Renderers/GUI/Window/GuiWindowStyle.h"
#include "Tests/Mocks/Api/InputManagerMock.h"
#include "Tests/Mocks/Gui/FontManagerMock.h"
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
        , guiRenderer(graphicsApiMock_)
    {
        EXPECT_CALL(resourcesManager_, GetTextureLoader()).WillRepeatedly(ReturnRef(textureLoaderMock));
        EXPECT_CALL(textureLoaderMock, CreateTexture(_, _, Matcher<Utils::Image&&>(_))).WillRepeatedly(Return(&texture));
        dialogueComponent = &obj_->AddComponent<Components::DialogueComponent>();

        auto createVerticalLayout = [&]() { return std::make_unique<VerticalLayout>(inputManagerMock_); };
        auto createGuiWindow      = [&](GuiWindowStyle style, const vec2& pos, const vec2& scale)
        { return std::make_unique<GuiWindowElement>(style, inputManagerMock_); };

        EXPECT_CALL(guiElementFactory_, CreateGuiWindow(_, _, _)).WillRepeatedly(Invoke(createGuiWindow));
        EXPECT_CALL(guiElementFactory_, CreateVerticalLayout()).WillRepeatedly(Invoke(createVerticalLayout));
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
        LOG_DEBUG << "TearDown";
        guiRenderer.UnSubscribeAll();
    }

    void expectGuiTextCreation(const std::string& text)
    {
        auto createGuiText = [&]()
        { return std::make_unique<GuiTextElement>(fontManagerMock, guiRenderer, resourcesManager_, text); };

        EXPECT_CALL(guiElementFactory_, CreateGuiText(text)).WillOnce(Invoke(createGuiText));
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

    FontManagerMock fontManagerMock;
    TextureLoaderMock textureLoaderMock;
    DialogueManager sut;
    GeneralTexture texture;
    Components::DialogueComponent* dialogueComponent;
    GUIRenderer guiRenderer;
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
    node.npcText    = "npcText text 1";
    node.options    = {};

    DialogueNode node2;
    node.id         = nextId;
    node.nextNodeID = -1;
    node.npcText    = "npcText text 2";
    node.options    = {};

    Components::DialogueComponent::Nodes nodes{{currentId, node}, {nextId, node2}};
    dialogueComponent->setNodes(std::move(nodes));
    dialogueComponent->startNodeID = currentId;
    dialogueComponent->resetCurrent();

    std::function<void()> timerCallback;
    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&timerCallback), Return(1)));
    expectGuiTextCreation(obj_->GetName());
    expectGuiTextCreation(node.npcText);

    sut.startDialogue(*obj_, *dialogueComponent);
    timerCallback();

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 1);
}

TEST_F(DialogueManagerTests, ShouldTransitionToCorrectNodeWhenOptionIsSelected)
{
    demo();

    const auto& nodes              = dialogueComponent->GetDialog();
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstTimer;
    std::function<void()> secondTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&secondTimer), Return(2)));

    expectGuiTextCreation(obj_->GetName());
    expectGuiTextCreation(nodes.at(0).npcText);

    sut.startDialogue(*obj_, *dialogueComponent);

    expectGuiTextCreation(nodes.at(0).options[0].text);
    expectGuiTextCreation(nodes.at(0).options[1].text);
    firstTimer();

    expectGuiTextCreation(obj_->GetName());
    expectGuiTextCreation(nodes.at(2).npcText);

    sut.selectOption(0);

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 2);
    EXPECT_EQ(dialogueComponent->getCurrent()->npcText, nodes.at(2).npcText);
}
