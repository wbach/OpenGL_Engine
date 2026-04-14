#include <GameEngine/Components/Camera/CameraComponent.h>
#include <GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h>
#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/ComponentFactory.h>
#include <GameEngine/Components/Dialogue/DialogueComponent.h>
#include <GameEngine/Narrative/Dialogs/DialogueManager.h>
#include <GameEngine/Narrative/Dialogs/DialogueOption.h>
#include <GameEngine/Narrative/Dialogs/Fsm/DialogEvents.h>
#include <GameEngine/Narrative/GameState.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Renderer.h>
#include <GameEngine/Renderers/GUI/Text/Text.h>
#include <GameEngine/Renderers/GUI/Window/Window.h>
#include <GameEngine/Renderers/GUI/Window/WindowStyle.h>
#include <Logger/Log.h>
#include <Renderers/GUI/Manager.h>
#include <Scene/TweenManager.h>
#include <Types.h>
#include <Utils/IdPool.h>
#include <Utils/Image/Image.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <memory>
#include <string>

#include "Tests/Mocks/Api/InputManagerMock.h"
#include "Tests/Mocks/Gui/FontManagerMock.h"
#include "Tests/Mocks/Gui/GuiElementFactoryMock.h"
#include "Tests/Mocks/Resources/TextureLoaderMock.h"
#include "Tests/Mocks/Utils/TimerServiceMock.h"
#include "Tests/UT/Components/BaseComponent.h"

using namespace std::chrono_literals;
using namespace testing;
using namespace GameEngine;

class DialogueManagerTests : public BaseComponentTestSchould
{
protected:
    DialogueManagerTests()
        : texture(graphicsApiMock_, Utils::Image{}, {}, {})
        , guiRenderer(graphicsApiMock_)
    {
        npcGameObject =
            std::make_unique<GameEngine::GameObject>("Npc", componentController_, componentFactory_, gameObjectIdPool);

        dialogueComponent = &npcGameObject->AddComponent<Components::DialogueComponent>();

        EXPECT_CALL(resourcesManager_, GetTextureLoader()).WillRepeatedly(ReturnRef(textureLoaderMock));
        EXPECT_CALL(textureLoaderMock, CreateTexture(_, _, Matcher<Utils::Image&&>(_))).WillRepeatedly(Return(&texture));

        auto createVerticalLayout = [&]() { return std::make_unique<GUI::VerticalLayout>(); };
        auto createWindow      = [&](GUI::WindowStyle style)
        { return std::make_unique<GUI::Window>(style); };

        EXPECT_CALL(guiElementFactory_, createWindow(_)).WillRepeatedly(Invoke(createWindow));
        EXPECT_CALL(guiElementFactory_, createVerticalLayout()).WillRepeatedly(Invoke(createVerticalLayout));

        ON_CALL(fontManagerMock, openFont(_, _, _, _)).WillByDefault(Return(1));
        ON_CALL(fontManagerMock, renderText(_, _, _)).WillByDefault(Return(GUI::TextureData{}));
        ON_CALL(fontManagerMock, closeFont(_)).WillByDefault(Return());
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
        auto createGuiText = [&](const std::string& t)
        { return std::make_unique<GUI::Text>(fontManagerMock, resourcesManager_, guiRenderer, t); };

        EXPECT_CALL(guiElementFactory_, createText(text)).WillOnce(Invoke(createGuiText));
    }

    void expectGuiTextCreation(const std::string& text, GUI::Text*& outPtr)
    {
        auto createGuiText = [&](const std::string& t) mutable
        {
            auto result = std::make_unique<GUI::Text>(fontManagerMock, resourcesManager_, guiRenderer, t);
            outPtr      = result.get();
            return result;
        };

        EXPECT_CALL(guiElementFactory_, createText(text)).WillOnce(Invoke(createGuiText));
    }

    void createPlayerGameObjectWitoutCamera()
    {
        playerGameObject =
            std::make_unique<GameEngine::GameObject>("Player", componentController_, componentFactory_, gameObjectIdPool);
    }

    void createPlayerGameObjectWithCamera()
    {
        auto cameraGo =
            std::make_unique<GameEngine::GameObject>("Camera", componentController_, componentFactory_, gameObjectIdPool);

        cameraGo->AddComponent<CameraComponent>();
        cameraGo->SetLocalPosition(vec3(1, 1, 1));

        GameObjects children;
        children.push_back(std::move(std::move(cameraGo)));

        playerGameObject = std::make_unique<GameEngine::GameObject>("Player", componentController_, componentFactory_,
                                                                    gameObjectIdPool, std::move(children));

        playerGameObject->AddComponent<ThridPersonCameraComponent>();
        playerGameObject->SetLocalPosition(vec3(0, 0, 1));
    }

    void demo()
    {
        Components::DialogueComponent::Nodes nodes;

        nodes[0] = {
            .id                  = 0,
            .nextNodeID          = 0,
            .text                = "Stoj! Kto idzie?",
            .setGameStateflag    = "",
            .removeGameStateFlag = "",
            .options             = {
                            DialogueOption{.text = "Szukam schronienia.", .nextNodeID = 2, .setGameStateflag = "", .removeGameStateFlag = ""},
                            DialogueOption{
                                .text = "Nie Twoj interes.", .nextNodeID = -1, .setGameStateflag = "", .removeGameStateFlag = ""}}};

        nodes[1] = {
            .id                  = 1,
            .nextNodeID          = 0,
            .text                = "O co chodzi?",
            .setGameStateflag    = "",
            .removeGameStateFlag = "",
            .options             = {
                            DialogueOption{.text = "Co to za oboz?", .nextNodeID = 2, .setGameStateflag = "", .removeGameStateFlag = ""},
                            DialogueOption{
                                .text = "Powiedz mi wiecej o obozie.", .nextNodeID = 3, .setGameStateflag = "", .removeGameStateFlag = ""},
                            DialogueOption{.text = "A nic.", .nextNodeID = -1, .setGameStateflag = "", .removeGameStateFlag = ""}}};

        nodes[2] = {
            .id                  = 2,
            .nextNodeID          = 0,
            .text                = "To Oboz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?",
            .setGameStateflag    = "",
            .removeGameStateFlag = "",
            .options             = {
                            DialogueOption{.text = "Tak, opowiedz mi.", .nextNodeID = 3, .setGameStateflag = "", .removeGameStateFlag = ""},
                            DialogueOption{
                                .text = "Nie, musze leciec.", .nextNodeID = -1, .setGameStateflag = "", .removeGameStateFlag = ""}}};

        nodes[3] = {
            .id                  = 3,
            .nextNodeID          = 0,
            .text                = "Duzo by gadac..., ale wydajesz sie porzadny pogadamy w srodku.",
            .setGameStateflag    = "",
            .removeGameStateFlag = "",
            .options             = {DialogueOption{
                            .text = "Dziekuje!", .nextNodeID = -2, .setGameStateflag = "invited_to_camp", .removeGameStateFlag = ""}}};

        nodes[4] = {
            .id                  = 4,
            .nextNodeID          = 0,
            .text                = "Do zobaczenia w srodku.",
            .setGameStateflag    = "",
            .removeGameStateFlag = "",
            .options             = {DialogueOption{
                            .text = "Do zobaczenia", .nextNodeID = -2, .setGameStateflag = "invited_to_camp", .removeGameStateFlag = ""}}};

        dialogueComponent->setNodes(std::move(nodes));
    }
    void expectGotToIdle()
    {
        // enddialog move to transition state
        dialogueManager_.processEvents();
        // enddialog move to idle state
        dialogueManager_.processEvents();

        EXPECT_FALSE(dialogueManager_.isActive());
    }

    NiceMock<GUI::FontManagerMock> fontManagerMock;
    TextureLoaderMock textureLoaderMock;
    GeneralTexture texture;
    Components::DialogueComponent* dialogueComponent;
    GUI::Renderer guiRenderer;
    std::string font{"arial.ttf"};

    std::unique_ptr<GameObject> playerGameObject;
    std::unique_ptr<GameObject> npcGameObject;
};

TEST_F(DialogueManagerTests, ShouldNotStartDialogueWhenComponentIsEmpty)
{
    createPlayerGameObjectWitoutCamera();

    dialogueComponent->setNodes({});
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);

    EXPECT_FALSE(dialogueManager_.isActive());
}

TEST_F(DialogueManagerTests, NpcAndPlayerInTheSamePosThenTweenShouldBeNotStarted)
{
    createPlayerGameObjectWithCamera();
    playerGameObject->SetLocalPosition(vec3(0, 0, 1));
    npcGameObject->SetLocalPosition(vec3(0, 0, 1));

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = -1;
    node.text       = "npcText text 1";
    node.options    = {};

    dialogueComponent->setNodes({{node.id, node}});
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    EXPECT_CALL(tweenManager, Add(_, _, _, _, _)).Times(0);

    std::function<void()> firstTimer;
    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.text);

    LOG_DEBUG << "Start dialog";
    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // StartSentence event

    firstTimer();

    expectGotToIdle();
}

TEST_F(DialogueManagerTests, NpcShouldStartRotationToPlayerAndCameraShouldStartMoveToDialogPos)
{
    createPlayerGameObjectWithCamera();

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = -1;
    node.text       = "npcText text 1";
    node.options    = {};

    dialogueComponent->setNodes({{node.id, node}});
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();
    std::function<void()> npcRotationTween;
    std::function<void()> cameraMovementTween;
    std::function<void()> npcRestorationRotationTween;

    EXPECT_CALL(tweenManager, Add(_, _, _, _, _))
        .WillOnce(SaveArg<4>(&npcRotationTween))
        .WillOnce(SaveArg<4>(&cameraMovementTween))
        .WillOnce(SaveArg<4>(&npcRestorationRotationTween));

    std::function<void()> firstTimer;
    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)));

    LOG_DEBUG << "Start dialog";
    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();

    EXPECT_EQ(npcRotationTween, nullptr);

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.text);

    LOG_DEBUG << "Camera movment ends";
    cameraMovementTween();
    dialogueManager_.processEvents();  // StartSentence event after camera move

    LOG_DEBUG << "Show dialog timers end";
    firstTimer();

    EXPECT_EQ(npcRestorationRotationTween, nullptr);

    expectGotToIdle();
}

TEST_F(DialogueManagerTests, ShouldAutomaticallyGoToNextNodeWhenNoOptionsAvailable)
{
    createPlayerGameObjectWitoutCamera();

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = 1;
    node.text       = "npcText text 1";
    node.options    = {};

    DialogueNode node2;
    node2.id         = 1;
    node2.nextNodeID = 2;
    node2.text       = "npcText text 2";
    node2.options    = {};

    DialogueNode node3;
    node3.id      = 2;
    node3.text    = "npcText text 3";
    node3.options = {DialogueOption{.text = "Dziekuje!", .nextNodeID = -1, .setGameStateflag = "invited_to_camp"}};

    Components::DialogueComponent::Nodes nodes{{node.id, node}, {node2.id, node2}, {node3.id, node3}};
    dialogueComponent->setNodes(std::move(nodes));
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstTimer;
    std::function<void()> secondTimer;
    std::function<void()> thridTimer;
    std::function<void()> playerResponseTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&secondTimer), Return(2)))
        .WillOnce(DoAll(SaveArg<1>(&thridTimer), Return(3)))
        .WillOnce(DoAll(SaveArg<1>(&playerResponseTimer), Return(4)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.text);

    LOG_DEBUG << "Start dialog";
    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // StartSentence event camera not exist

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node2.text);

    LOG_DEBUG << "First timer expiry";
    firstTimer();

    dialogueManager_.processEvents();  // StartSentence event - update

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 1);

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node3.text);

    LOG_DEBUG << "Seconds timer expiry";
    secondTimer();

    LOG_DEBUG << "processEvents";
    dialogueManager_.processEvents();  // StartSentence event - update

    LOG_DEBUG << "getCurrent";
    EXPECT_EQ(dialogueComponent->getCurrent()->id, 2);

    LOG_DEBUG << "optionguiText";
    GUI::Text* optionguiText{nullptr};
    expectGuiTextCreation(node3.options[0].text, optionguiText);

    LOG_DEBUG << "thridTimer";
    thridTimer();
    dialogueManager_.processEvents();  // StartSentence event - update

    LOG_DEBUG << "expectedighlighetColor";
    const vec4 expectedighlighetColor(1, 1, 0, 1);
    EXPECT_TRUE(glm::all(glm::epsilonEqual(optionguiText->getColor().value, expectedighlighetColor, 0.001f)));

    LOG_DEBUG << optionguiText->getColor();
    EXPECT_TRUE(optionguiText->isActive());

    expectGuiTextCreation(playerGameObject->GetName());
    expectGuiTextCreation(node3.options[0].text);

    // dialogueManager_.selectOption(0);
    dialogueManager_.handleEvent(OptionSelected{.option           = dialogueComponent->getCurrent()->options[0],
                                                .playerGameObject = *playerGameObject,
                                                .component        = *dialogueComponent});

    playerResponseTimer();

    expectGotToIdle();
}

TEST_F(DialogueManagerTests, ShouldTransitionToCorrectNodeWhenOptionIsSelected)
{
    createPlayerGameObjectWitoutCamera();

    demo();

    const auto& nodes = dialogueComponent->GetDialog();

    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstTimer;
    std::function<void()> playerResposeTimer;
    std::function<void()> secondNpcText;

    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&playerResposeTimer), Return(2)))
        .WillOnce(DoAll(SaveArg<1>(&secondNpcText), Return(3)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(nodes.at(0).text);

    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // StartSentence event camera not exist

    expectGuiTextCreation(nodes.at(0).options[0].text);
    expectGuiTextCreation(nodes.at(0).options[1].text);
    firstTimer();

    dialogueManager_.processEvents();  // Wait for input

    auto selectedOpiton = dialogueComponent->getCurrent()->options[0];
    expectGuiTextCreation(playerGameObject->GetName());
    expectGuiTextCreation(selectedOpiton.text);

    dialogueManager_.handleEvent(
        OptionSelected{.option = selectedOpiton, .playerGameObject = *playerGameObject, .component = *dialogueComponent});

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(nodes.at(2).text);

    playerResposeTimer();
    dialogueManager_.processEvents();  // StartSentence update to npc text

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 2);
    EXPECT_EQ(dialogueComponent->getCurrent()->text, nodes.at(2).text);

    // secondNpcText(); itd
}

TEST_F(DialogueManagerTests, ShouldShowOptionOnlyWhenAllConditionsAreMet)
{
    createPlayerGameObjectWitoutCamera();

    DialogueNode node;
    node.id   = 0;
    node.text = "Witaj w obozie.";

    DialogueOption secretOption;
    secretOption.text       = "Mam list do dowódcy.";
    secretOption.nextNodeID = 10;
    secretOption.conditions = {{"has_letter", ConditionType::REQUIRED}, {"is_traitor", ConditionType::FORBIDDEN}};

    DialogueOption normalOption;
    normalOption.text       = "Tylko przechodzę.";
    normalOption.nextNodeID = -1;

    node.options = {secretOption, normalOption};
    dialogueComponent->setNodes({{node.id, node}});
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    gameState_.setFlag("has_letter", false);
    gameState_.setFlag("is_traitor", false);

    std::function<void()> firstTimer;
    std::function<void()> playerResposeTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&playerResposeTimer), Return(2)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.text);

    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // StartSentence event camera not exist

    expectGuiTextCreation(normalOption.text);

    LOG_DEBUG << "Odpalamy timer - sprawdzamy czy tylko jedna opcja się pojawi";
    firstTimer();
    dialogueManager_.processEvents();  // Wait for input

    expectGuiTextCreation(playerGameObject->GetName());
    expectGuiTextCreation(normalOption.text);

    dialogueManager_.handleEvent(
        OptionSelected{.option = normalOption, .playerGameObject = *playerGameObject, .component = *dialogueComponent});

    dialogueManager_.processEvents();  // Player StartSentence

    playerResposeTimer();

    expectGotToIdle();

    gameState_.setFlag("has_letter", true);

    Mock::VerifyAndClearExpectations(&guiElementFactory_);

    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(2)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.text);

    LOG_DEBUG << "Odpalamy timer - sprawdzamy 2 opcje";
    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // StartSentence event camera not exist

    expectGuiTextCreation(secretOption.text);
    expectGuiTextCreation(normalOption.text);

    firstTimer();

    dialogueManager_.processEvents();
}

TEST_F(DialogueManagerTests, ShouldBackToNodeWithOptions)
{
    createPlayerGameObjectWitoutCamera();

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = 1;
    node.text       = "Hej! To jest super miejsce.";
    node.options    = {DialogueOption{.text             = "Powiedz mi wiecej!",
                                      .nextNodeID       = 1,
                                      .setGameStateflag = "ask_for_more",
                                      .conditions       = {{"ask_for_more", ConditionType::FORBIDDEN}}},
                       DialogueOption{.text = "Ok spdam", .nextNodeID = -1, .setGameStateflag = "dialog_end"}};

    DialogueNode node2;
    node2.id           = 1;
    node2.backToNodeID = 0;
    node2.text         = "Mamy wiele fajnych miejsc. Sam zobacz";
    node2.options      = {};

    Components::DialogueComponent::Nodes nodes{{node.id, node}, {node2.id, node2}};
    dialogueComponent->setNodes(std::move(nodes));
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstSentance;
    std::function<void()> playerResponseTimer;
    std::function<void()> secondNodeSentanceTimer;
    std::function<void()> thridTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstSentance), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&playerResponseTimer), Return(2)))
        .WillOnce(DoAll(SaveArg<1>(&secondNodeSentanceTimer), Return(3)));
    // .WillOnce(DoAll(SaveArg<1>(&playerResponseTimer), Return(4)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.text);

    LOG_DEBUG << "Start dialog";
    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // StartSentence event camera not exist

    // expectGuiTextCreation(npcGameObject->GetName());0--
    for (auto& node : node.options)
    {
        expectGuiTextCreation(node.text);
    }

    LOG_DEBUG << "First timer expiry";
    firstSentance();                   // Npc end sentance 1, and show options after end
    dialogueManager_.processEvents();  // Move to waiting input

    auto selectedOption = dialogueComponent->getCurrent()->options[0];

    expectGuiTextCreation(playerGameObject->GetName());
    expectGuiTextCreation(selectedOption.text);

    dialogueManager_.handleEvent(
        OptionSelected{.option = selectedOption, .playerGameObject = *playerGameObject, .component = *dialogueComponent});

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node2.text);

    playerResponseTimer();
    dialogueManager_.processEvents();

    // after back to node 0 , only 1 options left, because of flags ask_for_more
    expectGuiTextCreation(node.options[1].text);

    secondNodeSentanceTimer();

    // optionsSelet > move to showing
    dialogueManager_.processEvents();  // Back event move to waiting input

    // Player sentence finish
    dialogueManager_.processEvents();
}

TEST_F(DialogueManagerTests, ShouldBackToNodeAfterPlayerOptionSentance)
{
    createPlayerGameObjectWitoutCamera();

    DialogueNode node0;
    node0.id      = 0;
    node0.text    = "Witaj podróżniku. Co chcesz wiedzieć?";
    node0.options = {DialogueOption{.text             = "Opowiedz o okolicy.",
                                    .nextNodeID       = -1,
                                    .backToNodeID     = 0,  // Po tej opcji wróć tutaj
                                    .setGameStateflag = "asked_about_area",
                                    .conditions       = {{"asked_about_area", ConditionType::FORBIDDEN}}},
                     DialogueOption{.text = "Bywaj.", .nextNodeID = -1}};

    Components::DialogueComponent::Nodes nodes{{node0.id, node0}};
    dialogueComponent->setNodes(std::move(nodes));
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> npcStartTimer;
    std::function<void()> playerSpeechTimer;

    // Oczekujemy dwóch timerów: 1. NPC mówi start, 2. Gracz mówi wybraną opcję
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&npcStartTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&playerSpeechTimer), Return(2)));

    // 1. Start dialogu - NPC mówi tekst startowy
    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node0.text);

    dialogueManager_.startDialogue(*playerGameObject, *dialogueComponent);
    dialogueManager_.processEvents();  // Wejście w ShowingSentence (NPC)

    // Po zakończeniu kwestii NPC, pojawiają się 2 opcje
    expectGuiTextCreation(node0.options[0].text);
    expectGuiTextCreation(node0.options[1].text);

    npcStartTimer();
    dialogueManager_.processEvents();  // Przejście do WaitingForInput

    // 2. Gracz wybiera opcję "Opowiedz o okolicy"
    auto selectedOption = node0.options[0];

    // Teraz Gracz mówi swoją kwestię (ShowingSentence dla gracza)
    expectGuiTextCreation(playerGameObject->GetName());
    expectGuiTextCreation(selectedOption.text);

    dialogueManager_.handleEvent(
        OptionSelected{.option = selectedOption, .playerGameObject = *playerGameObject, .component = *dialogueComponent});
    dialogueManager_.processEvents();

    // 3. Koniec kwestii gracza -> Powrót do węzła 0
    // Oczekujemy, że tekst NPC się NIE pojawi, a od razu pojawi się TYLKO druga opcja
    // (bo pierwsza ma flagę FORBIDDEN, która właśnie została ustawiona)
    expectGuiTextCreation(node0.options[1].text);

    playerSpeechTimer();               // Kończymy gadanie gracza
    dialogueManager_.processEvents();  // FSM powinno wysłać BackToSentence i wejść w WaitingForInput

    EXPECT_TRUE(dialogueManager_.isActive());
}