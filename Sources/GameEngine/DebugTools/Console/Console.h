#pragma once
#include <Types.h>

#include <Mutex.hpp>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <Input/KeysSubscriptionsManager.h>
#include "GameEngine/Camera/FirstPersonCamera.h"

namespace GameEngine
{
class Scene;
class GameObject;
class GuiTextElement;
class GuiWindowElement;

namespace Debug
{
class Console
{
public:
    Console(Scene& scene);
    ~Console();
    void ExecuteCommands();

private:
    std::string GetCommandNameFromString(const std::string&) const;
    std::vector<std::string> GetParams(const std::string& comannd) const;
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
    void CameraInfo(const std::vector<std::string>&);
    void SetFreeCamera(const std::vector<std::string>&);
    void DisableFreeCam(const std::vector<std::string>&);
    void LoadPrefab(const std::vector<std::string>&);
    void SetPosition(const std::vector<std::string>&);
    void PrintPosition(const std::vector<std::string>&);
    void SaveScene(const std::vector<std::string>&);
    void LoadScene(const std::vector<std::string>&);
    void ReloadScene(const std::vector<std::string>&);
    void SetImmeditalyLogs(const std::vector<std::string>&);
    void TakeSnapshoot(const std::vector<std::string>&);
    void ReloadShaders(const std::vector<std::string>&);
    void SwapRenderMode(const std::vector<std::string>&);
    void EnableEditorNetworkInterface(const std::vector<std::string>&);
    void Help(const std::vector<std::string>&);

private:
    Scene& scene_;
    GuiWindowElement* window_;
    std::list<GuiTextElement*> guiTexts_;
    std::vector<std::string> commandsHistory_;
    std::list<std::string> commandsToExecute_;
    std::mutex commandsExecuteMutex_;
    GuiTextElement* currentCommand_;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandsActions_;
    int32 commandHistoryIndex_;
    ICamera* stashedCamera_;
    std::unique_ptr<FirstPersonCamera> firstPersonCamera_;

private:
    Input::KeysSubscriptionsManager keysSubscribtionManager_;
};
}  // namespace Debug
}  // namespace GameEngine
