#pragma once
#include <TreeNode.h>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/File.h"
namespace GameEngine
{
namespace GUI
{
class Window;
class VerticalLayout;
}  // namespace GUI

class GameState;
class Quest;

namespace Components
{
class ENGINE_API QuestViewerComponent : public BaseComponent
{
public:
    File guiFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(guiFile)
    END_FIELDS()
    // clang-format on

public:
    QuestViewerComponent(ComponentContext&, GameObject&);
    ~QuestViewerComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void initGui();
    void updateGui();
    void updateContent(const Quest&);

private:
    GUI::Window* mainWindow{nullptr};
    GUI::VerticalLayout* questLayout{nullptr};
    GUI::VerticalLayout* contentLayout{nullptr};
    TreeNode questButtonTemplate;
    TreeNode questDescriptionText;

    const Quest* currentQuest{nullptr};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
