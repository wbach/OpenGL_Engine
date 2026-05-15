#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <TreeNode.h>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Renderers/GUI/Layer/Layer.h"
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
DECLARE_COMPONENT(QuestViewerComponent)
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
    Input::KeysSubscriptionsManager keySubManager;
    GUI::Layer* layer{nullptr};
    GUI::Element* group{nullptr};

    GUI::VerticalLayout* questLayout{nullptr};
    GUI::VerticalLayout* contentLayout{nullptr};
    TreeNode questButtonTemplate;
    TreeNode questDescriptionText;
    GUI::Button* activeQuestButton{nullptr};

    const Quest* currentQuest{nullptr};

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
