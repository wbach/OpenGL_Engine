#pragma once
#include <GameEngine/Narrative/Dialogs/DialogueNode.h>

#include <cstddef>
#include <optional>
#include <vector>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Resources/File.h"
#include "Logger/Log.h"
#include "Rotation.h"
#include "Types.h"

namespace GameEngine
{
class GameState;
namespace Components
{
class ENGINE_API DialogueComponent : public BaseComponent
{
public:
    using Nodes = std::unordered_map<int, DialogueNode>;
    File dialogueFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(dialogueFile)
    END_FIELDS()
    // clang-format on

public:
    DialogueComponent(ComponentContext&, GameObject&);
    ~DialogueComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;
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

    void RotateObjectToPlayer(const vec3&);
    void RestoreRotation();

    const DialogueNode* getCurrent() const;
    const DialogueOption* selectOption(size_t);
    const DialogueNode* goToNode(int);
    void setNodes(Nodes&&);
    void resetCurrent();
    const std::string& GetName() const;
    int getStartingNode() const;

private:
    Nodes nodes;
    int currentNodeID = 0;
    std::optional<Rotation> tmpRotation;
    std::string dialogName;
    std::vector<int> entryPoints;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
