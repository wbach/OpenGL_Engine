#include "DialogueManager.h"

#include <Logger/Log.h>

#include <utility>

#include "GameEngine/Renderers/GUI/Layout/Layout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"

namespace GameEngine
{
DialogueManager::DialogueManager(GuiElementFactory& factory, GuiManager& manger, GameState& gs)
    : guiFactory(factory)
    , guiManager(manger)
    , gameState(gs)
{
    setupDemo();
}
void DialogueManager::startDialogue(const std::string& npcName, const std::string& file, int nodeId)
{
    LOG_DEBUG << "StartDialogue file: " << file;
    if (nodes.find(nodeId) != nodes.end())
    {
        currentNodeID    = nodeId;
        isDialogueActive = true;

        initGui();
        textWindowLayout->RemoveAll();
        optionsWindowLayout->RemoveAll();

        if (auto current = getCurrent())
        {
            const vec2 textSize{0.25f, 0.2};
            auto npcNameGuiText = guiFactory.CreateGuiText(npcName);
            npcNameGuiText->SetLocalScale(textSize + vec2(0.1f, 0.1f));
            textWindowLayout->AddChild(std::move(npcNameGuiText));

            auto npcGuiText = guiFactory.CreateGuiText(current->npcText);
            npcGuiText->SetLocalScale(textSize);
            textWindowLayout->AddChild(std::move(npcGuiText));

            for (const auto& option : current->options)
            {
                auto optionGuiText = guiFactory.CreateGuiText("-- " + option.text);
                optionGuiText->SetLocalScale(textSize);
                optionGuiText->SetAlgin(GuiTextElement::Algin::LEFT);
                optionsWindowLayout->AddChild(std::move(optionGuiText));
            }
        }
    }
    else
    {
        LOG_ERROR << "Dialogue Node " << nodeId << " not found!";
    }
}
void DialogueManager::setupDemo()
{
    // Węzeł 1: Powitanie
    nodes[1] = {1, "Stoj! Kto idzie?", {{"Szukam schronienia.", 2, ""}, {"Nie Twoj interes.", -1, ""}}};
    nodes.at(currentNodeID);
    // Węzeł 2: Opowieść o swiecie
    nodes[2] = {2,
                "To Obóz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?",
                {{"Tak, opowiedz mi.", 3, ""}, {"Nie, musze leciec.", -1, ""}}};

    // Węzeł 3: Zaproszenie
    nodes[3] = {3, "Wydajesz sie porzadny. Wejdz do srodka.", {{"Dziekuje!", -1, "invited_to_camp"}}};
}
void DialogueManager::selectOption(int optionIndex)
{
    DialogueOption& selected = nodes[currentNodeID].options[optionIndex];
    if (!selected.actionFlag.empty())
    {
        gameState.flags[selected.actionFlag] = 1;
    }

    if (selected.nextNodeID == -1)
    {
        EndDialog();
    }
    else
    {
        currentNodeID = selected.nextNodeID;
    }
}
bool DialogueManager::isActive() const
{
    return isDialogueActive;
}
const DialogueNode* DialogueManager::getCurrent() const
{
    auto iter = nodes.find(currentNodeID);
    if (iter != nodes.end())
    {
        return &iter->second;
    }

    return nullptr;
}
void DialogueManager::EndDialog()
{
    currentNodeID    = 1;
    isDialogueActive = false;
}
void DialogueManager::initGui()
{
    auto createWindowWithLayout = [&](const vec2& position, const vec2& scale)
    {
        auto dialogueWindowPtr  = guiFactory.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, position, scale);
        auto textDialogueWindow = dialogueWindowPtr.get();
        auto layout             = guiFactory.CreateVerticalLayout();
        auto textWindowLayout   = layout.get();
        dialogueWindowPtr->AddChild(std::move(layout));
        guiManager.Add(std::move(dialogueWindowPtr));

        return std::make_pair(textDialogueWindow, textWindowLayout);
    };
    if (not textDialogueWindow)
    {
        const vec2& position{0.5f, 0.75f};
        const vec2& scale{0.5f, 0.2f};
        auto [window, layout] = createWindowWithLayout(position, scale);
        textDialogueWindow    = window;
        textWindowLayout      = layout;
    }

    if (not optionsDialogueWindow)
    {
        const vec2& position{0.5f, 0.25f};
        const vec2& scale{0.95f, 0.24f};
        auto [window, layout] = createWindowWithLayout(position, scale);
        layout->SetAlgin(Layout::Algin::LEFT);
        optionsDialogueWindow = window;
        optionsWindowLayout   = layout;
    }
}
}  // namespace GameEngine
