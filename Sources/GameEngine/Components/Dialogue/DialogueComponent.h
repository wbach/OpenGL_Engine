#pragma once
#include <GameEngine/Dialogs/DialogueNode.h>

#include <optional>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Resources/File.h"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
class GameState;
namespace Components
{
class DialogueComponent : public BaseComponent
{
public:
    enum class SelectOptionResult
    {
        active,
        end
    };
    using Nodes = std::unordered_map<int, DialogueNode>;
    File dialogueFile;
    int startNodeID{0};

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(dialogueFile)
        FIELD_INT(startNodeID)
    END_FIELDS()
    // clang-format on

public:
    DialogueComponent(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void readFile();

public:
    const Nodes& GetDialog() const
    {
        return nodes;
    }

    int GetDialogNodeId() const
    {
        return currentNodeID;
    }

    const DialogueNode* getCurrent() const;
    SelectOptionResult selectOption(int optionIndex);

private:
    Nodes nodes;
    int currentNodeID = 0;

    // /GameState& gameState;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
