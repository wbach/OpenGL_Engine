#pragma once
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <GameEngine/Resources/File.h>
#include <Input/KeysSubscriptionsManager.h>
#include <Types.h>

#include <Mutex.hpp>
#include <functional>
#include <memory>
#include <vector>

#include "Rotation.h"

namespace GameEngine
{
namespace GUI
{
class Manager;
class VerticalLayout;
class Button;
class Window;
class Element;
class IElementFactory;
}  // namespace GUI

class Scene;
namespace Components
{
class ComponentContext;
class PlayerInputController;


class PauseMenu
{
public:
    enum class State
    {
        MainMenu,
        PauseMenu,
        SettingsView,
        SceneLoader
    };

    class View
    {
    public:
        using GuiButtons   = std::vector<GameEngine::GUI::Button*>;
        using ButtonsArray = std::vector<GuiButtons>;

        struct Buttons
        {
            ButtonsArray array;
            int selectedRow{-1};
            int selectedColumn{0};

            void updateSelectedRow(int);
            void updateSelectedColumn(int);
            bool isInRange() const;
            GuiButtons& createRow();
            void highLightCurrent();
            void toneDownCurrent();
            void exectuteAction();
            GameEngine::GUI::Button& get();
        };

        struct Child
        {
            GameEngine::GUI::Window* window{nullptr};
            Buttons buttons;
        };

        View()
        {
            resetFocus();
        }

        Buttons& getActiveButtons()
        {
            return *selectedArray;
        }

        void setFocusToChildrenArray(Child& child)
        {
            selectedArray = &child.buttons;
            focusToChild  = true;
        }

        void resetFocus()
        {
            selectedArray = &buttonsArray;
            focusToChild  = false;
        }

        bool isFocusToChild() const
        {
            return focusToChild;
        }

    public:
        GameEngine::GUI::VerticalLayout* layout{nullptr};
        std::unordered_map<std::string, Child> children;

    private:
        Buttons buttonsArray;
        Buttons* selectedArray;
        bool focusToChild{false};
    };

    PauseMenu(GameEngine::File&, State, GameEngine::Components::ComponentContext&, const vec2& = {1.f, 0.075f});
    PauseMenu(GameEngine::File&, State, GameEngine::Components::ComponentContext&, const vec2&,
              const std::unordered_map<std::string, uint32>&);
    ~PauseMenu();

    void show();
    void hide();
    bool isShow() const;
    State getCurrentState() const;

private:
    void subscribeKeys();
    void enableState(State);
    void backToStartState();
    void enableMainMenuState();
    void enablePauseMenuState();
    void enableSettingsViewState();
    void enableSceneLoaderViewState();
    void init();
    void setMainWindowVerticalLayoutTransform(GameEngine::GUI::VerticalLayout&);
    void createMainMenuLayout();
    void createPauseMenuLayout();
    void createSettingsView();
    void createSceneLoaderLayout();
    void fillSettingsParamWindow(View::Child&, const std::string&);

    void showSettingWindow(View&, const std::string&);
    void createMessageBox(const std::string&);
    void setViewAsActive(State);

    void createButtonForLayout(View&, const std::string&, std::function<void()>);

private:
    GameEngine::Components::ComponentContext& context;
    GameEngine::File& logoFile;
    bool onePramaterNeedRestart_{false};
    State startState_;

    GameEngine::Scene& scene_;
    GameEngine::GUI::IElementFactory& factory_;
    GameEngine::GUI::Manager& guiManager_;

    GameEngine::GUI::Window* messageBox_;
    GameEngine::GUI::Window* mainWindow_;

    std::unordered_map<State, View> views_;

    std::pair<const State, View>* currentView_;

    GameEngine::ConfigurationExplorer configurationExplorer_;

    std::mutex eventMutex_;
    std::vector<std::function<void()>> events_;

    const vec2 menuButtonSize_;
    std::unordered_map<std::string, uint32> avaiableScenes_;

    std::vector<std::pair<uint32, GameEngine::Params::IConfigurationParam*>> paramChangeSubs_;

    Input::KeysSubscriptionsManager keysSubscriptionsManager;
    uint32 escapeSub;

    Rotation cameraRotationBeforeShow;
    vec3 cameraPositionBeforShow;
    std::set<GameEngine::Components::PlayerInputController*> playerComponentToRestore;
};

}  // namespace Components
}  // namespace GameEngine
