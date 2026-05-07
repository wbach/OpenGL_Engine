#pragma once
#include <TreeNode.h>

#include "GameEngine/Components/ComponentCore.h"
#include "GameEngine/Resources/File.h"
namespace GameEngine
{
namespace GUI
{
class Window;
class VerticalLayout;
class Button;
}  // namespace GUI

class GameState;
class Quest;

namespace Components
{
class ENGINE_API QuestViewerComponent : public ComponentCore
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
    void show();
    void hide();

private:
    GUI::Window* mainWindow{nullptr};
    GUI::VerticalLayout* questLayout{nullptr};
    GUI::VerticalLayout* contentLayout{nullptr};
    TreeNode questButtonTemplate;
    TreeNode questDescriptionText;
    GUI::Button* activeQuestButton{nullptr};

    const Quest* currentQuest{nullptr};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
