#pragma once
#include <Input/KeysSubscriptionsManager.h>

#include <string>
#include <unordered_map>

#include "DialogueNode.h"
#include "EngineApi.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/IGuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Scene/TweenTransform.h"
#include "GameState.h"

namespace Utils::Time
{
class ITimerService;
}

namespace Input
{
class InputManager;
}

namespace GameEngine
{
class GuiWindowElement;
class VerticalLayout;
class ITweenManager;
namespace Components
{
class DialogueComponent;
class ThridPersonCameraComponent;
class CameraComponent;
}  // namespace Components

class ENGINE_API DialogueManager
{
public:
    DialogueManager(Utils::Time::ITimerService&, Input::InputManager&, IGuiElementFactory&, GuiManager&, GameState&,
                    ITweenManager&);

    void startDialogue(GameObject&, Components::DialogueComponent&);

    void selectOption(int optionIndex);
    void goToNode(int);
    bool isActive() const;
    void EndDialog();

private:
    void initGui();
    void updateHighLightedColor(int oldItem, int newItem);
    void refreshOptionGui();
    std::optional<TweenTransform> calculateCameraTarget();

    void hideOptions();
    void showOptions();
    std::vector<std::pair<int, DialogueOption>> getVisibleOptions(const DialogueNode&) const;

private:
    Utils::Time::ITimerService& timerService;
    Input::InputManager& inputManager;
    IGuiElementFactory& guiFactory;
    GuiManager& guiManager;
    GuiWindowElement* textDialogueWindow{nullptr};
    VerticalLayout* textWindowLayout{nullptr};

    GuiWindowElement* optionsDialogueWindow{nullptr};
    VerticalLayout* optionsWindowLayout{nullptr};

    Input::KeysSubscriptionsManager subscriptions_;

private:
    GameState& gameState;
    ITweenManager& tweenManager;
    GameObject* gameObject{nullptr};
    Components::DialogueComponent* dialogueComponent{nullptr};
    Components::ThridPersonCameraComponent* thridPersonCameraComponent{nullptr};
    Components::CameraComponent* cameraComponent{nullptr};
    std::vector<std::pair<int, DialogueOption>> visibleOptions;
    int highlighted{0};
    std::string npcName;
};
}  // namespace GameEngine
