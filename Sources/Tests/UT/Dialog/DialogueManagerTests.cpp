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

#include "Components/Camera/CameraComponent.h"
#include "Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
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
        npcGameObject =
            std::make_unique<GameEngine::GameObject>("Npc", componentController_, componentFactory_, gameObjectIdPool);

        dialogueComponent = &npcGameObject->AddComponent<Components::DialogueComponent>();

        EXPECT_CALL(resourcesManager_, GetTextureLoader()).WillRepeatedly(ReturnRef(textureLoaderMock));
        EXPECT_CALL(textureLoaderMock, CreateTexture(_, _, Matcher<Utils::Image&&>(_))).WillRepeatedly(Return(&texture));

        auto createVerticalLayout = [&]() { return std::make_unique<VerticalLayout>(inputManagerMock_); };
        auto createGuiWindow      = [&](GuiWindowStyle style, const vec2& pos, const vec2& scale)
        { return std::make_unique<GuiWindowElement>(style, inputManagerMock_); };

        EXPECT_CALL(guiElementFactory_, CreateGuiWindow(_, _, _)).WillRepeatedly(Invoke(createGuiWindow));
        EXPECT_CALL(guiElementFactory_, CreateVerticalLayout()).WillRepeatedly(Invoke(createVerticalLayout));

        ON_CALL(fontManagerMock, openFont(_, _)).WillByDefault(Return(1));
        ON_CALL(fontManagerMock, renderFont(_, _, _)).WillByDefault(Return(IFontManager::TextureData{}));
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
        { return std::make_unique<GuiTextElement>(fontManagerMock, guiRenderer, resourcesManager_, font, t); };

        EXPECT_CALL(guiElementFactory_, CreateGuiText(text)).WillOnce(Invoke(createGuiText));
    }

    void expectGuiTextCreation(const std::string& text, GuiTextElement*& outPtr)
    {
        auto createGuiText = [&](const std::string& t) mutable
        {
            auto result = std::make_unique<GuiTextElement>(fontManagerMock, guiRenderer, resourcesManager_, font, t);
            outPtr      = result.get();
            return result;
        };

        EXPECT_CALL(guiElementFactory_, CreateGuiText(text)).WillOnce(Invoke(createGuiText));
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

    sut.startDialogue(*playerGameObject, *dialogueComponent);

    EXPECT_FALSE(sut.isActive());
}

TEST_F(DialogueManagerTests, NpcAndPlayerInTheSamePosThenTweenShouldBeNotStarted)
{
    createPlayerGameObjectWithCamera();
    playerGameObject->SetLocalPosition(vec3(0, 0, 1));
    npcGameObject->SetLocalPosition(vec3(0, 0, 1));

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = -1;
    node.npcText    = "npcText text 1";
    node.options    = {};

    dialogueComponent->setNodes({{node.id, node}});
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    EXPECT_CALL(tweenManager, Add(_, _, _, _, _)).Times(0);

    std::function<void()> firstTimer;
    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.npcText);

    LOG_DEBUG << "Start dialog";
    sut.startDialogue(*playerGameObject, *dialogueComponent);

    firstTimer();

    EXPECT_FALSE(sut.isActive());
}

TEST_F(DialogueManagerTests, NpcShouldStartRotationToPlayerAndCameraShouldStartMoveToDialogPos)
{
    createPlayerGameObjectWithCamera();

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = -1;
    node.npcText    = "npcText text 1";
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
    sut.startDialogue(*playerGameObject, *dialogueComponent);

    EXPECT_EQ(npcRotationTween, nullptr);

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.npcText);

    LOG_DEBUG << "Camera movment ends";
    cameraMovementTween();

    LOG_DEBUG << "Show dialog timers end";
    firstTimer();

    EXPECT_EQ(npcRestorationRotationTween, nullptr);

    EXPECT_FALSE(sut.isActive());
}

TEST_F(DialogueManagerTests, ShouldAutomaticallyGoToNextNodeWhenNoOptionsAvailable)
{
    createPlayerGameObjectWitoutCamera();

    DialogueNode node;
    node.id         = 0;
    node.nextNodeID = 1;
    node.npcText    = "npcText text 1";
    node.options    = {};

    DialogueNode node2;
    node2.id         = 1;
    node2.nextNodeID = 2;
    node2.npcText    = "npcText text 2";
    node2.options    = {};

    DialogueNode node3;
    node3.id = 2;
    // node3.nextNodeID = -1;
    node3.npcText = "npcText text 3";
    node3.options = {{"Dziekuje!", -2, "invited_to_camp"}};

    Components::DialogueComponent::Nodes nodes{{node.id, node}, {node2.id, node2}, {node3.id, node3}};
    dialogueComponent->setNodes(std::move(nodes));
    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstTimer;
    std::function<void()> secondTimer;
    std::function<void()> thridTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&secondTimer), Return(2)))
        .WillOnce(DoAll(SaveArg<1>(&thridTimer), Return(3)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.npcText);

    LOG_DEBUG << "Start dialog";
    sut.startDialogue(*playerGameObject, *dialogueComponent);

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node2.npcText);

    LOG_DEBUG << "First timer expiry";
    firstTimer();

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 1);

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node3.npcText);

    LOG_DEBUG << "Seconds timer expiry";
    secondTimer();

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 2);

    GuiTextElement* optionguiText{nullptr};
    expectGuiTextCreation(node3.options[0].text, optionguiText);

    thridTimer();

    const vec4 expectedighlighetColor(1, 1, 0, 1);
    EXPECT_TRUE(glm::all(glm::epsilonEqual(optionguiText->GetColor(), expectedighlighetColor, 0.001f)));

    LOG_DEBUG << optionguiText->GetColor();
    EXPECT_TRUE(optionguiText->IsShow());

    sut.selectOption(0);

    EXPECT_FALSE(sut.isActive());
}

TEST_F(DialogueManagerTests, ShouldTransitionToCorrectNodeWhenOptionIsSelected)
{
    createPlayerGameObjectWitoutCamera();

    demo();

    const auto& nodes = dialogueComponent->GetDialog();

    dialogueComponent->startNodeID = 0;
    dialogueComponent->resetCurrent();

    std::function<void()> firstTimer;
    std::function<void()> secondTimer;
    EXPECT_CALL(timerService_, timer(_, _))
        .WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)))
        .WillOnce(DoAll(SaveArg<1>(&secondTimer), Return(2)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(nodes.at(0).npcText);

    sut.startDialogue(*playerGameObject, *dialogueComponent);

    expectGuiTextCreation(nodes.at(0).options[0].text);
    expectGuiTextCreation(nodes.at(0).options[1].text);
    firstTimer();

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(nodes.at(2).npcText);

    sut.selectOption(0);

    EXPECT_EQ(dialogueComponent->getCurrent()->id, 2);
    EXPECT_EQ(dialogueComponent->getCurrent()->npcText, nodes.at(2).npcText);
}

TEST_F(DialogueManagerTests, ShouldShowOptionOnlyWhenAllConditionsAreMet)
{
    createPlayerGameObjectWitoutCamera();

    DialogueNode node;
    node.id      = 0;
    node.npcText = "Witaj w obozie.";

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

    gameState_.flags["has_letter"] = false;
    gameState_.flags["is_traitor"] = false;

    std::function<void()> firstTimer;
    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(1)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.npcText);

    sut.startDialogue(*playerGameObject, *dialogueComponent);

    expectGuiTextCreation(normalOption.text);

    LOG_DEBUG << "Odpalamy timer - sprawdzamy czy tylko jedna opcja się pojawi";
    firstTimer();

    gameState_.flags["has_letter"] = true;

    Mock::VerifyAndClearExpectations(&guiElementFactory_);

    EXPECT_CALL(timerService_, timer(_, _)).WillOnce(DoAll(SaveArg<1>(&firstTimer), Return(2)));

    expectGuiTextCreation(npcGameObject->GetName());
    expectGuiTextCreation(node.npcText);

    sut.startDialogue(*playerGameObject, *dialogueComponent);

    expectGuiTextCreation(secretOption.text);
    expectGuiTextCreation(normalOption.text);

    firstTimer();
}