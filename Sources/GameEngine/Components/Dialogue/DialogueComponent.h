#pragma once
#include <optional>


#include "GameEngine/Components/BaseComponent.h"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class DialogueComponent : public BaseComponent
{
public:
    std::string dialogueFile;
    int startNodeID;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_STRING(dialogueFile)
        FIELD_INT(startNodeID)
    END_FIELDS()
    // clang-format on

public:
    DialogueComponent(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
