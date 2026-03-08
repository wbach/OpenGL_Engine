#include "DialogueManager.h"

#include <Logger/Log.h>

#include <utility>

#include "GameEngine/Renderers/GUI/Layout/Layout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "Input/InputManager.h"
#include "Input/KeyCodes.h"

namespace GameEngine
{
namespace
{
const vec3 highlightedColor(1, 1, 0);
}
DialogueManager::DialogueManager(Input::InputManager& inputManager, GuiElementFactory& factory, GuiManager& manger, GameState& gs)
    : inputManager(inputManager)
    , guiFactory(factory)
    , guiManager(manger)
    , subscriptions_(inputManager)
    , gameState(gs)
{
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::UARROW,
                                                     [&]()
                                                     {
                                                         if (auto current = getCurrent())
                                                         {
                                                             int oldItem = highlighted;
                                                             highlighted--;
                                                             if (highlighted < 0)
                                                             {
                                                                 highlighted = current->options.size() - 1;
                                                             }
                                                             int newItem = highlighted;
                                                             updateHighLightedColor(oldItem, newItem);
                                                         }
                                                     });
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::DARROW,
                                                     [&]()
                                                     {
                                                         if (auto current = getCurrent())
                                                         {
                                                             int oldItem = highlighted;
                                                             highlighted++;
                                                             if (highlighted >= static_cast<int>(current->options.size()))
                                                             {
                                                                 highlighted = 0;
                                                             }
                                                             int newItem = highlighted;
                                                             updateHighLightedColor(oldItem, newItem);
                                                         }
                                                     });
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::ENTER,
                                                     [&]()
                                                     {
                                                         if (isActive())
                                                         {
                                                             selectOption(highlighted);
                                                             highlighted = 0;
                                                         }
                                                     });

    setupDemo();
}
void DialogueManager::startDialogue(const std::string& npcName, const std::string& file, int nodeId)
{
    this->npcName = npcName;

    LOG_DEBUG << "StartDialogue file: " << file;
    if (nodes.find(nodeId) != nodes.end())
    {
        currentNodeID    = nodeId;
        isDialogueActive = true;

        initGui();
        refreshOptionGui();
    }
    else
    {
        LOG_ERROR << "Dialogue Node " << nodeId << " not found!";
    }
}
void DialogueManager::setupDemo()
{
    nodes[1] = {1, "Stoj! Kto idzie?", {{"Szukam schronienia.", 2, ""}, {"Nie Twoj interes.", -1, ""}}};
    nodes.at(currentNodeID);
    nodes[2] = {2,
                "To Oboz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?",
                {{"Tak, opowiedz mi.", 3, ""}, {"Nie, musze leciec.", -1, ""}}};
    nodes[3] = {3, "Wydajesz sie porzadny. Wejdz do srodka.", {{"Dziekuje!", -1, "invited_to_camp"}}};
}
void DialogueManager::selectOption(int optionIndex)
{
    LOG_DEBUG << "selectOption " << optionIndex;

    DialogueOption& selected = nodes[currentNodeID].options[optionIndex];
    if (!selected.actionFlag.empty())
    {
        gameState.flags[selected.actionFlag] = 1;
    }

    if (selected.nextNodeID == -1)
    {
        LOG_DEBUG << "End dialog";
        EndDialog();
    }
    else
    {
        LOG_DEBUG << "nextNodeID " << selected.nextNodeID;
        currentNodeID = selected.nextNodeID;
        refreshOptionGui();
    }
}
bool DialogueManager::isActive() const
{
    return isDialogueActive;
}
const DialogueNode* DialogueManager::getCurrent() const
{
    if (not isActive())
        return nullptr;

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

    if (textDialogueWindow)
        textDialogueWindow->Hide();
    if (optionsDialogueWindow)
        optionsDialogueWindow->Hide();
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
    else
    {
        textDialogueWindow->Show();
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
    else
    {
        optionsDialogueWindow->Show();
    }
}
void DialogueManager::updateHighLightedColor(int oldItem, int newItem)
{
    auto& children = optionsWindowLayout->GetChildren();
    if (!optionsWindowLayout || children.empty())
        return;

    auto updateColor = [&](int index, glm::vec3 color)
    {
        if (index >= 0 && index < static_cast<int>(children.size()))
        {
            if (auto* text = dynamic_cast<GuiTextElement*>(children[index].get()))
            {
                text->SetColor(color);
            }
        }
    };

    updateColor(oldItem, vec3(1, 1, 1));
    updateColor(newItem, highlightedColor);
}
void DialogueManager::refreshOptionGui()
{
    textWindowLayout->RemoveAll();
    optionsWindowLayout->RemoveAll();

    if (auto current = getCurrent())
    {
        const vec2 textSize{1.0f, 0.25};
        auto npcNameGuiText = guiFactory.CreateGuiText(npcName);
        npcNameGuiText->SetLocalScale(textSize + vec2(0.1f, 0.1f));
        textWindowLayout->AddChild(std::move(npcNameGuiText));

        auto npcGuiText = guiFactory.CreateGuiText(current->npcText);
        npcGuiText->SetLocalScale(textSize);
        textWindowLayout->AddChild(std::move(npcGuiText));

        int i = 0;
        for (const auto& option : current->options)
        {
            auto optionGuiText = guiFactory.CreateGuiText("-- " + option.text);
            optionGuiText->SetLocalScale(textSize);
            optionGuiText->SetAlgin(GuiTextElement::Algin::LEFT);
            if (i == 0)
            {
                optionGuiText->SetColor(highlightedColor);
                i++;
            }
            optionsWindowLayout->AddChild(std::move(optionGuiText));
        }
    }
}
}  // namespace GameEngine
