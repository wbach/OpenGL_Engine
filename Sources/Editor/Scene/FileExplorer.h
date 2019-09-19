#pragma once
#include <GameEngine/Renderers/GUI/GuiElementFactory.h>
#include <GameEngine/Renderers/GUI/GuiManager.h>
#include <functional>
#include <string>

namespace GameEngine
{
class VerticalLayout;
class GuiTextElement;
}  // namespace GameEngine

namespace Editor
{
class FileExplorer
{
public:
    FileExplorer(GameEngine::GuiManager&, GameEngine::GuiElementFactory&);
    ~FileExplorer();
    void Start(const std::string&, std::function<bool(const std::string&)>);

private:
    void CreateWindowWithLayout(const vec2& position, const vec2& size);
    void FillFileList(const std::string& dir);
    void AddRefillTask(const std::string& dir);
    void CreateButtonWithFilename(const std::string&, GameEngine::ActionFunction);
    void CreateCurrentDirBar();
    void CreateFileListLayout();
    void CreateSeletedFileText();
    void CreateActionButtons();

private:
    GameEngine::GuiManager& guiManager_;
    GameEngine::GuiElementFactory& guiFactory_;
    std::string font_;
    GameEngine::GuiEditBoxElement* seletedFileText_;
    GameEngine::GuiWindowElement* window_;
    GameEngine::GuiEditBoxElement* currentDir_;
    GameEngine::VerticalLayout* fileLayout_;
    GameEngine::VerticalLayout* windowLayout_;
    std::function<bool(const std::string&)> onChoose_;
};
}  // namespace Editor
