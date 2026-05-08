#include "PauseMenuComponent.h"

#include <GameEngine/Components/ComponentType.h>
#include <GameEngine/Components/ComponentsReadFunctions.h>
#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include <GameEngine/Scene/Scene.hpp>
#include <optional>

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_START_STATE[] = "startState";
constexpr char CSTR_BUTTON_SIZE[] = "buttonSize";
constexpr char CSTR_LOGO_FILE[]   = "logo";
constexpr char CSTR_BACKGROUND[]  = "background";
constexpr char CSTR_FONT[]        = "font";
constexpr char CSTR_FONT_SIZE[]   = "fontSize";
}  // namespace

REGISTER_COMPONENT(PauseMenuComponent)

PauseMenuComponent::PauseMenuComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
    , buttonSize({1.f, 0.075f})
    , font("GUI/herculanum.ttf")
    , fontSize(50)
    , pauseMenuImage("GUI/1200px-Avatar_The_Last_Airbender_logo.svg.png")
    , mainMenuBackground("GUI/204473.jpg")
    , startState{PauseMenu::State::MainMenu}
    , backgroundTexture{nullptr}
{
}

void PauseMenuComponent::CleanUp()
{
    LOG_DEBUG << "Clean up PauseMenuComponent";
    pauseMenu.reset();
    if (backgroundTexture)
    {
        componentContext_.guiManager_.remove(*backgroundTexture);
        backgroundTexture = nullptr;
    }
    LOG_DEBUG << "Clean up done";
}

void PauseMenuComponent::ReqisterFunctions()
{
    LOG_DEBUG << "Hello from PauseMenuComponent";
    RegisterFunction(FunctionType::Awake, [this]() { Init(); });
}

void PauseMenuComponent::Reload()
{
    CleanUp();
    Init();
}

void PauseMenuComponent::Init()
{
    if (pauseMenu)
    {
        LOG_DEBUG << "Menu already created. return";
        return;
    }

    auto& guiElementFactory = componentContext_.guiElementFactory_;

    GameEngine::GUI::Theme guiTheme;
    guiTheme.text.font                        = font.GetDataRelativePath().string();
    guiTheme.text.fontSize_                   = std::nullopt;
    guiTheme.window.background.color.value.w  = 0.5f;
    guiTheme.button.normal.sprite.baseTexture = std::nullopt;

    guiElementFactory.setTheme(guiTheme);
    const auto& avaiableScenes = componentContext_.sceneManager_.GetAvaiableScenes();

    pauseMenu = std::make_unique<PauseMenu>(pauseMenuImage, startState, componentContext_, buttonSize, avaiableScenes);

    if (startState == PauseMenu::State::MainMenu)
    {
        auto background = guiElementFactory.createSprite(mainMenuBackground.GetAbsolutePath().string());
        background->setZPosition(100.f);
        backgroundTexture = background.get();
        componentContext_.guiManager_.add(std::move(background));
        pauseMenu->show();
    }
}
void PauseMenuComponent::read(const TreeNode& node)
{
    ::Read(node.getChild(CSTR_LOGO_FILE), pauseMenuImage);
    ::Read(node.getChild(CSTR_BACKGROUND), mainMenuBackground);
    ::Read(node.getChild(CSTR_FONT), font);
    ::Read(node.getChild(CSTR_BUTTON_SIZE), buttonSize);
    ::Read(node.getChild(CSTR_FONT_SIZE), fontSize);
    ::Read(node.getChild(CSTR_START_STATE), startState);
}
void PauseMenuComponent::write(TreeNode& node) const
{
    node.addChild(CSTR_LOGO_FILE, pauseMenuImage.GetDataRelativePath());
    node.addChild(CSTR_BACKGROUND, mainMenuBackground.GetDataRelativePath());
    node.addChild(CSTR_FONT, font.GetDataRelativePath());
    node.addChild(CSTR_FONT_SIZE, std::to_string(fontSize));
    node.addChild(CSTR_BUTTON_SIZE, std::to_string(buttonSize));
    node.addChild(CSTR_START_STATE, magic_enum::enum_name(startState));
}
}  // namespace Components
}  // namespace GameEngine
