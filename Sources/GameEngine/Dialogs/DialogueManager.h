#pragma once
#include <unordered_map>

#include "DialogueNode.h"
#include "GameState.h"
#include "Logger/Log.h"

namespace GameEngine
{
class DialogueManager
{
public:
    struct DialogueView
    {
        DialogueNode node;
        int highlightedOption;
    };

    DialogueManager(GameState& gs)
        : gameState(gs)
    {
        setupDemo();
    }

    void startDialogue(const std::string& file, int nodeId = 1)
    {
        LOG_DEBUG << "StartDialogue file: " << file;
        if (nodes.find(nodeId) != nodes.end())
        {
            currentNodeID    = nodeId;
            isDialogueActive = true;
        }
        else
        {
            LOG_ERROR << "Dialogue Node " << nodeId << " not found!";
        }
    }
    void setupDemo()
    {
        // Węzeł 1: Powitanie
        nodes[1] = {1,
                    "Stoj! Kto idzie?",
                    {
                        {"Szukam schronienia.", 2, ""}, {"Nie Twoj interes.", -1, ""}  // -1 oznacza koniec rozmowy
                    }};
        nodes.at(currentNodeID);
        // Węzeł 2: Opowieść o swiecie
        nodes[2] = {2,
                    "To Obóz Cienia. Swiat oszalal. Chcesz wiedziec wiecej?",
                    {{"Tak, opowiedz mi.", 3, ""}, {"Nie, musze leciec.", -1, ""}}};

        // Węzeł 3: Zaproszenie
        nodes[3] = {3,
                    "Wydajesz sie porzadny. Wejdz do srodka.",
                    {
                        {"Dziekuje!", -1, "invited_to_camp"}  // Ustawia flagę przy wyjściu
                    }};
    }

    void selectOption(int optionIndex)
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

    bool isActive() const
    {
        return isDialogueActive;
    }

    const DialogueNode* getCurrent() const
    {
        auto iter = nodes.find(currentNodeID);
        if (iter != nodes.end())
        {
            return &iter->second;
        }

        return nullptr;
    }

    void EndDialog()
    {
        currentNodeID    = 1;
        isDialogueActive = false;
    }

private:
    std::unordered_map<int, DialogueNode> nodes;
    int currentNodeID = 1;
    GameState& gameState;
    bool isDialogueActive{false};
};
}  // namespace GameEngine