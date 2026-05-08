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
DECLARE_COMPONENT(CharacterStatsComponent)
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
    void updateGuiStats();
    void show();
    void hide();

public:
    Gothic::CharacterStats characterStats;

private:
    GUI::Window* mainWindow{nullptr};

    std::unordered_map<std::string, GUI::Text*> params;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
