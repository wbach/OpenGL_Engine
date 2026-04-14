#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace GUI
{
class Window;
}

class GameState;
class Quest;

namespace Components
{
class ENGINE_API QuestViewerComponent : public BaseComponent
{
public:
public:
    // clang-format off
    BEGIN_FIELDS()
    END_FIELDS()
    // clang-format on

public:
    QuestViewerComponent(ComponentContext&, GameObject&);
    ~QuestViewerComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void createWindow();

private:
    GUI::Window* mainWindow{nullptr};
    const Quest* currentQuest{nullptr};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
