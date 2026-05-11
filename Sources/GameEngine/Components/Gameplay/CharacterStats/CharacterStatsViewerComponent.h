#pragma once
#include <TreeNode.h>

#include <unordered_map>

#include "CharacterStats.h"
#include "GameEngine/Components/Component.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
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
class CharacterStatsComponent;

DECLARE_COMPONENT(CharacterStatsViewerComponent)
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
    CharacterStatsViewerComponent(ComponentContext&, GameObject&);
    ~CharacterStatsViewerComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void initGui();
    void updateGui();
    void updateGuiStats();
    void show();
    void hide();

private:
    CharacterStatsComponent* characterStats{nullptr};
    GUI::Window* mainWindow{nullptr};

    std::unordered_map<std::string, GUI::Text*> params;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
