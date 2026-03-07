#pragma once
#include <unordered_map>

#include "DialogueNode.h"
#include "EngineApi.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameState.h"

namespace GameEngine
{
class GuiWindowElement;
class VerticalLayout;

class ENGINE_API DialogueManager
{
public:
    DialogueManager(GuiElementFactory&, GuiManager&, GameState& gs);

    void startDialogue(const std::string& npcName, const std::string& file, int nodeId = 1);
    void setupDemo();
    void selectOption(int optionIndex);
    bool isActive() const;
    const DialogueNode* getCurrent() const;
    void EndDialog();

private:
    void initGui();

private:
    GuiElementFactory& guiFactory;
    GuiManager& guiManager;
    GuiWindowElement* textDialogueWindow{nullptr};
    VerticalLayout* textWindowLayout{nullptr};

    GuiWindowElement* optionsDialogueWindow{nullptr};
    VerticalLayout* optionsWindowLayout{nullptr};

private:
    std::unordered_map<int, DialogueNode> nodes;
    int currentNodeID = 1;
    GameState& gameState;
    bool isDialogueActive{false};
};
}  // namespace GameEngine