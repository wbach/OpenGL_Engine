#pragma once
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <Types.h>

#include <Mutex.hpp>
#include <functional>
#include <memory>

namespace GameEngine
{
class Scene;
class GuiManager;
class VerticalLayout;
class GuiWindowElement;
class GuiElementFactory;
}  // namespace GameEngine

namespace AvatarGame
{
class PauseMenu
{
public:
    enum class State
    {
        MainMenu,
        PauseMenu,
        SettingsView
    };

    PauseMenu(GameEngine::Scene&, GameEngine::GuiElementFactory&, GameEngine::GuiManager&);
    ~PauseMenu();

    void show(State);
    void hide();
    bool isShow() const;

private:
    void enableState(State);
    void backToStartState();
    void enableMainMenuState();
    void enablePauseMenuState();
    void enableSettingsViewState();
    void init();
    void setMainWindowVerticalLayoutTransform(GameEngine::VerticalLayout&);
    std::unique_ptr<GameEngine::VerticalLayout> createMainMenuLayout();
    std::unique_ptr<GameEngine::VerticalLayout> createPauseMenuLayout();
    std::unique_ptr<GameEngine::VerticalLayout> createSettingsLayout();
    void fillSettingsParamWindow(GameEngine::VerticalLayout&, const std::string&);
    void hideSettingWindows();
    void showSettingWindow(const std::string&);
    void createMessageBox(const std::string&);

private:
    bool onePramaterNeedRestart_{false};
    State startState_;

    GameEngine::Scene& scene_;
    GameEngine::GuiElementFactory& factory_;
    GameEngine::GuiManager& guiManager_;

    GameEngine::GuiWindowElement* messageBox_;
    GameEngine::GuiWindowElement* mainWindow_;
    GameEngine::VerticalLayout* settingsLayout_;
    GameEngine::VerticalLayout* pauseMenuLayout_;
    GameEngine::VerticalLayout* mainMenuLayout_;
    GameEngine::ConfigurationExplorer configurationExplorer_;

    std::unordered_map<std::string, GameEngine::GuiWindowElement*> settingWindows_;

    std::mutex eventMutex_;
    std::vector<std::function<void()>> events_;

    const vec2& menuButtonSize_;
};
}  // namespace AvatarGame
