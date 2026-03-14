#pragma once
#include <Input/KeysSubscriptionsManager.h>

#include <string>
#include <unordered_map>

#include "DialogueNode.h"
#include "EngineApi.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Scene/TweenManager.h"
#include "GameState.h"
namespace Input
{
class InputManager;
}

namespace GameEngine
{
class GuiWindowElement;
class VerticalLayout;
namespace Components
{
class DialogueComponent;
class ThridPersonCameraComponent;
class CameraComponent;
}  // namespace Components

class ENGINE_API DialogueManager
{
public:
    DialogueManager(Input::InputManager&, GuiElementFactory&, GuiManager&, GameState&, TweenManager&);

    void startDialogue(GameObject&, Components::DialogueComponent&);

    void selectOption(int optionIndex);
    bool isActive() const;
    void EndDialog();

private:
    void initGui();
    void updateHighLightedColor(int oldItem, int newItem);
    void refreshOptionGui();
    TweenTransform calculateCameraTarget();

private:
    Input::InputManager& inputManager;
    GuiElementFactory& guiFactory;
    GuiManager& guiManager;
    GuiWindowElement* textDialogueWindow{nullptr};
    VerticalLayout* textWindowLayout{nullptr};

    GuiWindowElement* optionsDialogueWindow{nullptr};
    VerticalLayout* optionsWindowLayout{nullptr};

    Input::KeysSubscriptionsManager subscriptions_;

private:
    GameState& gameState;
    TweenManager& tweenManager;
    GameObject* gameObject{nullptr};
    Components::DialogueComponent* dialogueComponent{nullptr};
    Components::ThridPersonCameraComponent* thridPersonCameraComponent{nullptr};
    Components::CameraComponent* cameraComponent{nullptr};
    int highlighted{0};
    std::string npcName;
};
}  // namespace GameEngine
