#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <Types.h>

#include <Mutex.hpp>
#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace GameEngine
{
class Scene;
class GameObject;
class GuiTextElement;
class GuiWindowElement;
class VerticalLayout;
class ICamera;

namespace Debug
{
class Console
{
public:
    using Params = std::vector<std::string>;

    Console(Scene&);
    ~Console();
    void ExecuteCommands();

private:
    std::string GetCommandNameFromString(const std::string&) const;
    Params GetParams(const std::string& comannd) const;
    bool IsKnownCommand(const std::string&) const;

    void AddCommand(const std::string& command);
    void AddAndUpdateHistoryFileIfNeeded(const std::string& command);
    void ExecuteComand(const std::string& command);

    void AddCommandToExecute(const std::string&);
    std::string GetCommandToExecute();

    void MoveUpTexts();
    GuiTextElement* AddOrUpdateGuiText(const std::string& command);
    void PrintMsgInConsole(const std::string& msg);
    void CloseConsole();
    void SubscribeKeys();
    void RegisterActions();
    GameObject* GetGameObject(const std::string&);

    void PrepareConsoleWindow();
    void LoadCommandHistoryFromFile();

private:
    void CameraInfo(const Params&);
    void SetFreeCamera(const Params&);
    void DisableFreeCam(const Params&);
    void LoadPrefab(const Params&);
    void SetPosition(const Params&);
    void PrintPosition(const Params&);
    void SaveScene(const Params&);
    void LoadScene(const Params&);
    void Select(const Params&);
    void MoveTo(const Params&);
    void ReloadScene(const Params&);
    void UseAsyncLogging(const Params&);
    void TakeSnapshoot(const Params&);
    void ReloadShaders(const Params&);
    void SwapRenderMode(const Params&);
    void EnableEditorNetworkInterface(const Params&);
    void SetPhysicsVisualization(const Params&);
    void SetTimeMulitplayer(const Params&);
    void DebugRender(const Params&);
    void Help(const Params&);
    void Exit(const Params&);

private:
    Scene& scene_;
    GuiWindowElement* window_;
    VerticalLayout* windowVerticalLayout_;
    std::list<GuiTextElement*> guiTexts_;
    Params commandsHistory_;
    std::list<std::string> commandsToExecute_;
    std::mutex commandsExecuteMutex_;
    GuiTextElement* currentCommand_;
    std::unordered_map<std::string, std::function<void(const Params&)>> commandsActions_;
    int32 commandHistoryIndex_;
    std::optional<IdType> firstPersonCameraId_;
    ICamera* previousMainCamera_{nullptr};

private:
    Input::KeysSubscriptionsManager keysSubscribtionManager_;
};
}  // namespace Debug
}  // namespace GameEngine
