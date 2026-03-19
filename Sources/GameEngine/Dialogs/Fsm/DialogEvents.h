#pragma once
#include <cstddef>
#include <filesystem>
#include <string>
#include <variant>
#include <vector>

#include "GameEngine/Dialogs/DialogueNode.h"
#include "GameEngine/Dialogs/DialogueOption.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
class DialogueComponent;
}

struct StartRequested
{
    GameObject& playerGameObject;
    Components::DialogueComponent& component;
};

struct StartSentence
{
    DialogueNode dialogNode;
    GameObject& playerGameObject;
    Components::DialogueComponent& component;
};

struct StartInputWaiting
{
    std::vector<std::pair<int, DialogueOption>> visibleOptions;

    GameObject& playerGameObject;
    Components::DialogueComponent& component;
};

struct OptionSelected
{
    DialogueOption option;

    GameObject& playerGameObject;
    Components::DialogueComponent& component;
};

struct SkipRequested
{
};

struct AbortRequested
{
};

struct EndDialog
{
    GameObject& playerGameObject;
    Components::DialogueComponent& component;
};

using DialogEvent =
    std::variant<StartRequested, StartSentence, StartInputWaiting, OptionSelected, SkipRequested, AbortRequested, EndDialog>;
}  // namespace GameEngine
