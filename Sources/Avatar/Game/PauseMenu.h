#pragma once
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <Input/KeysSubscriptionsManager.h>
#include <Types.h>

#include <Mutex.hpp>
#include <functional>
#include <memory>

namespace GameEngine
{
class Scene;
class GuiManager;
class VerticalLayout;
class GuiElement;
class GuiWindowElement;
class GuiElementFactory;
class GuiButtonElement;
}  // namespace GameEngine

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
        using GuiButtons   = std::vector<GameEngine::GuiButtonElement*>;
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
            GameEngine::GuiButtonElement& get();
        };

        struct Child
        {
            GameEngine::GuiWindowElement* window{nullptr};
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
        GameEngine::VerticalLayout* layout{nullptr};
        std::unordered_map<std::string, Child> children;

    private:
        Buttons buttonsArray;
        Buttons* selectedArray;
        bool focusToChild{false};
    };

    PauseMenu(State, GameEngine::Scene&, GameEngine::GuiElementFactory&);
    PauseMenu(State, GameEngine::Scene&, GameEngine::GuiElementFactory&, const std::unordered_map<std::string, uint32>&);
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
    void setMainWindowVerticalLayoutTransform(GameEngine::VerticalLayout&);
    void createMainMenuLayout();
    void createPauseMenuLayout();
    void createSettingsView();
    void createSceneLoaderLayout();
    void fillSettingsParamWindow(View::Child&, const std::string&);

    void showSettingWindow(View&, const std::string&);
    void createMessageBox(const std::string&);
    void setViewAsActive(State);

    void createButtonForLayout(View&, const std::string&, std::function<void(GameEngine::GuiElement&)>);

private:
    bool onePramaterNeedRestart_{false};
    State startState_;

    GameEngine::Scene& scene_;
    GameEngine::GuiElementFactory& factory_;
    GameEngine::GuiManager& guiManager_;

    GameEngine::GuiWindowElement* messageBox_;
    GameEngine::GuiWindowElement* mainWindow_;

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
};
