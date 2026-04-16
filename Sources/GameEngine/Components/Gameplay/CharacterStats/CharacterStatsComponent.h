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
class Button;
}  // namespace GUI

class GameState;

namespace Components
{
class ENGINE_API CharacterStatsComponent : public BaseComponent
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
    CharacterStatsComponent(ComponentContext&, GameObject&);
    ~CharacterStatsComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void initGui();
    void updateGui();
    void show();
    void hide();

private:
    GUI::Window* mainWindow{nullptr};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
