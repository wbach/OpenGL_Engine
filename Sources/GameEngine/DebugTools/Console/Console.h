#pragma once
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace GameEngine
{
class Scene;
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
    void RegisterActions();
    void LoadPrefab(const std::vector<std::string>&);
    void PrintPosition(const std::vector<std::string>&);
    std::vector<std::string> GetParams(const std::string& comannd);
    void SubscribeKeys();

private:
    Scene& scene_;
    GuiWindowElement* window_;
    std::list<GuiTextElement*> guiTexts_;
    std::vector<std::string> commands_;
    GuiTextElement* currentCommand_;
    std::unordered_map<std::string, std::function<void (const std::vector<std::string>&)>> commandsActions_;
};
}  // namespace Debug
}  // namespace GameEngine
