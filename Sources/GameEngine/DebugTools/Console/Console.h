#pragma once
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

namespace Debug
{
class Console
{
public:
    Console(Scene& scene);

private:
    void AddCommand(const std::string& command);
    void ExecuteComand(const std::string& command);
    GuiTextElement* AddOrUpdateGuiText(const std::string& command);
    void MoveUpTexts();

private:
    void SubscribeKeys();
    void RegisterActions();
    void LoadPrefab(const std::vector<std::string>&);
    void SetPosition(const std::vector<std::string>&);
    void PrintPosition(const std::vector<std::string>&);
    void LoadScene(const std::vector<std::string>&);
    void ReloadScene(const std::vector<std::string>&);
    void SetImmeditalyLogs(const std::vector<std::string>&);
    void TakeSnapshoot(const std::vector<std::string>&);
    void ReloadShaders(const std::vector<std::string>&);
    GameObject* GetGameObject(const std::string&);
    std::vector<std::string> GetParams(const std::string& comannd);

private:
    Scene& scene_;
    GuiWindowElement* window_;
    std::list<GuiTextElement*> guiTexts_;
    std::vector<std::string> commands_;
    GuiTextElement* currentCommand_;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandsActions_;
};
}  // namespace Debug
}  // namespace GameEngine
