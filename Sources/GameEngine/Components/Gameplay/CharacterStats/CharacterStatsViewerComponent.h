#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <TreeNode.h>

#include <unordered_map>

#include "CharacterStats.h"
#include "GameEngine/Components/Component.h"
#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Resources/File.h"
namespace GameEngine
{
namespace GUI
{
class Window;
class VerticalLayout;
class Button;
class Sprite;
class Layer;
}  // namespace GUI

class GameState;

namespace Components
{
class CharacterStatsComponent;

DECLARE_COMPONENT(CharacterStatsViewerComponent)
{
public:
    File guiFile;
    File hudFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(guiFile)
        FIELD_FILE(hudFile)
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
    void initStatsPanel();
    void initHud();
    void updateGui();
    void updateGuiStats();
    void show();
    void hide();

private:
    Input::KeysSubscriptionsManager keySubManager;
    CharacterStatsComponent* characterStats{nullptr};
    GUI::Layer* layer{nullptr};
    GUI::Element* group{nullptr};

    GUI::Sprite* hpValue{nullptr};

    std::unordered_map<std::string, GUI::Text*> params;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
