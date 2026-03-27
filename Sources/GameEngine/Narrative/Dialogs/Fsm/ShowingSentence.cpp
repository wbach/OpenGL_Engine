#include "ShowingSentence.h"

#include <Input/KeyCodes.h>
#include <Logger/Log.h>
#include <Time/ITimerService.h>
#include <Time/TimerService.h>
#include <Utils/Variant.h>

#include <optional>
#include <variant>

#include "DialogContext.h"
#include "GameEngine/Audio/IAudioManager.h"
#include "GameEngine/Audio/PlayParameters.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Narrative/Dialogs/DialogueNode.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogEvents.h"
#include "GameEngine/Narrative/GameState.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/IGuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"

namespace GameEngine
{
namespace
{
const std::chrono::milliseconds calculateTimer(const std::string& text)
{
    const int baseTimeMs  = 1500;
    const int timePerChar = 35;
    const int maxTimeMs   = 8000;

    int calculatedTime = baseTimeMs + (static_cast<int>(text.length()) * timePerChar);

    calculatedTime = std::min(calculatedTime, maxTimeMs);

    LOG_DEBUG << "calculateTimer :  " << calculatedTime;

    return std::chrono::milliseconds(calculatedTime);
}
std::vector<std::pair<int, DialogueOption>> getVisibleOptions(const DialogueNode& node, const GameState& gameState)
{
    std::vector<std::pair<int, DialogueOption>> visibleOptions;

    int index = 0;
    for (const auto& option : node.options)
    {
        bool isAvailable = true;

        for (const auto& condition : option.conditions)
        {
            bool hasFlag = gameState.hasFlag(condition.flag);

            if (condition.type == ConditionType::REQUIRED and not hasFlag)
            {
                isAvailable = false;
                break;
            }
            if (condition.type == ConditionType::FORBIDDEN and hasFlag)
            {
                isAvailable = false;
                break;
            }
        }

        if (isAvailable)
        {
            visibleOptions.push_back({index, option});
        }

        ++index;
    }

    return visibleOptions;
}

template <typename T>
void updateGameStateFlags(GameState& gameState, const T& dialog)
{
    if (not dialog.setGameStateflag.empty())
    {
        gameState.setFlag(dialog.setGameStateflag, true);
    }
    if (not dialog.removeGameStateFlag.empty())
    {
        gameState.setFlag(dialog.removeGameStateFlag, false);
    }
}

template <typename T>
std::variant<AudioId, IdType, std::monostate> playAudioOrStartTimer(Utils::Time::ITimerService& timerService,
                                                                    IAudioManager& audioManager, const T& dialog,
                                                                    std::function<void()> callback)
{
    File audioFile(dialog.audioPath);
    if (audioFile.exist())
    {
        PlayParameters playParameters;
        playParameters.playEndCallback = callback;

        auto maybeResult = audioManager.play(audioFile, PlayGroup::Dialogs, playParameters);
        if (maybeResult != INVALID_AUDIO_ID)
        {
            return maybeResult;
        }
    }

    LOG_DEBUG << "Audio file \"" << dialog.audioPath << "\" not found for dialog: " << dialog.text;
    return timerService.timer(calculateTimer(dialog.text), std::move(callback));
}
}  // namespace

ShowingSentence::ShowingSentence(DialogContext& dialogContext)
    : dialogContext{dialogContext}
    , subscriptions_(dialogContext.inputManager)
{
}
void ShowingSentence::onEnter()
{
    dialogContext.sentenceWindow.window->Show();
    dialogContext.sentenceWindow.layout->RemoveAll();
    subscribeForInput();
}
void ShowingSentence::onEnter(const StartSentence& event)
{
    createGuiTexts(event.component.getParentGameObject().GetName(), event.dialogNode.text);
    updateGameStateFlags(dialogContext.gameState, event.dialogNode);
    setAnimations(event.playerGameObject, event.component.GetParentGameObject());

    auto sendNextEvent = [this, e = event]()
    {
        auto visibleOptions = getVisibleOptions(e.dialogNode, dialogContext.gameState);

        if (visibleOptions.empty())
        {
            if (e.dialogNode.nextNodeID != INVALID_NODE_ID)
            {
                if (auto nextNode = e.component.goToNode(e.dialogNode.nextNodeID))
                {
                    dialogContext.sendEvent(
                        StartSentence{.dialogNode = *nextNode, .playerGameObject = e.playerGameObject, .component = e.component});
                    return;
                }
            }
            if (e.dialogNode.backToNodeID != INVALID_NODE_ID)
            {
                if (auto backNode = e.component.goToNode(e.dialogNode.backToNodeID))
                {
                    auto visibleOptions = getVisibleOptions(*backNode, dialogContext.gameState);

                    if (not visibleOptions.empty())
                    {
                        dialogContext.sendEvent(BackToSentence{.visibleOptions   = std::move(visibleOptions),
                                                               .playerGameObject = e.playerGameObject,
                                                               .component        = e.component});
                    }
                    else
                    {
                        LOG_WARN << "Back to node is posssible only if options exising in that node";
                    }
                    return;
                }
            }

            dialogContext.sendEvent(EndDialog{.playerGameObject = e.playerGameObject, .component = e.component});
            return;
        }

        dialogContext.sendEvent(StartInputWaiting{
            .visibleOptions = std::move(visibleOptions), .playerGameObject = e.playerGameObject, .component = e.component});
    };

    playId = playAudioOrStartTimer(dialogContext.timerService, dialogContext.audioManager, event.dialogNode, sendNextEvent);
}
void ShowingSentence::createGuiTexts(const std::string& characterName, const std::string& sentance)
{
    const vec2 textSize{1.0f, 0.25};
    auto characterNameText = dialogContext.guiFactory.CreateGuiTextWrapped(characterName, 0);
    characterNameText->SetLocalScale(textSize + vec2(0.1f, 0.1f));

    dialogContext.sentenceWindow.layout->AddChild(std::move(characterNameText));

    auto sentanceGuiText = dialogContext.guiFactory.CreateGuiTextWrapped(sentance, WRAP_WIDTH);
    sentanceGuiText->SetLocalScale({1.0f, 0.4});
    dialogContext.sentenceWindow.layout->AddChild(std::move(sentanceGuiText));
}

void ShowingSentence::onEnter(const OptionSelected& event)
{
    createGuiTexts(event.playerGameObject.GetName(), event.option.text);
    updateGameStateFlags(dialogContext.gameState, event.option);
    setAnimations(event.component.GetParentGameObject(), event.playerGameObject);

    auto sendNextEvent = [this, e = event]()
    {
        if (e.option.nextNodeID != INVALID_NODE_ID)
        {
            if (auto nextNode = e.component.goToNode(e.option.nextNodeID))
            {
                dialogContext.sendEvent(
                    StartSentence{.dialogNode = *nextNode, .playerGameObject = e.playerGameObject, .component = e.component});
                return;
            }
        }

        if (e.option.backToNodeID != INVALID_NODE_ID)
        {
            if (auto backNode = e.component.goToNode(e.option.backToNodeID))
            {
                auto visibleOptions = getVisibleOptions(*backNode, dialogContext.gameState);
                dialogContext.sendEvent(BackToSentence{.visibleOptions   = std::move(visibleOptions),
                                                       .playerGameObject = e.playerGameObject,
                                                       .component        = e.component});
                return;
            }
        }

        dialogContext.sendEvent(EndDialog{.playerGameObject = e.playerGameObject, .component = e.component});
    };

    playId =
        playAudioOrStartTimer(dialogContext.timerService, dialogContext.audioManager, event.option, std::move(sendNextEvent));
}

void ShowingSentence::update(const StartSentence& event)
{
    dialogContext.sentenceWindow.layout->RemoveAll();
    onEnter(event);
};
void ShowingSentence::subscribeForInput()
{
    subscriptions_ =
        dialogContext.inputManager.SubscribeOnKeyDown(KeyCodes::ESCAPE, [this]() { dialogContext.sendEvent(SkipRequested{}); });
}
void ShowingSentence::onLeave()
{
    subscriptions_.UnsubscribeKeys();
}
void ShowingSentence::update(const SkipRequested&)
{
    std::visit(visitor{[&](GameEngine::AudioId id) { dialogContext.audioManager.finish(id); },
                       [&](IdType id) { dialogContext.timerService.finish(id); }, [](std::monostate) {}},
               playId);

    playId = std::monostate{};
}
void ShowingSentence::setAnimations(GameObject& listener, GameObject& speaker)
{
    setAnimation(listener, StartDialogEvent::Role::Listener);
    setAnimation(speaker, StartDialogEvent::Role::Speaker);
}
}  // namespace GameEngine
