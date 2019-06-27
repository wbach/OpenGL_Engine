#pragma once
#include <vector>
#include <list>
#include <string>

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
    Scene& scene_;
    GuiWindowElement* window_;
    std::list<GuiTextElement*> guiTexts_;
    std::vector<std::string> commands_;
    GuiTextElement* currentCommand_;
};
}  // namespace Debug
}  // namespace GameEngine
