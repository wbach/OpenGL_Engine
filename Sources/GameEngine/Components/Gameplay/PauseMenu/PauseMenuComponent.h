#pragma once
#include <GameEngine/Components/Component.h>

#include "GameEngine/Components/RegisterComponentWrapper.h"
#include "GameEngine/Resources/File.h"
#include "PauseMenu.h"

namespace GameEngine
{
namespace GUI
{
class Sprite;
}

namespace Components
{
struct ReadFunctions;

DECLARE_COMPONENT(PauseMenuComponent)
{
public:
    vec2 buttonSize;
    File font;
    uint32 fontSize;
    File pauseMenuImage;
    File mainMenuBackground;
    PauseMenu::State startState;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR2F(buttonSize)
        FIELD_FILE(font)
        //FIELD_UINT(fontSize)
        FIELD_TEXTURE(pauseMenuImage)
        FIELD_TEXTURE(mainMenuBackground)
        FIELD_ENUM(startState)
    END_FIELDS()

public:
    PauseMenuComponent(GameEngine::Components::ComponentContext&, GameEngine::GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

private:
    void Init();

    std::unique_ptr<PauseMenu> pauseMenu;
    GUI::Sprite* backgroundTexture;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}
}  // namespace GameEngine
