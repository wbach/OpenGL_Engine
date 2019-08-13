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
    void Start(const std::string&, std::function<void(const std::string&)>);

private:
    void FillFileList(GameEngine::VerticalLayout& layout, const std::string& dir,
                      std::function<void(const std::string&)>);
    void CreateButtonWithFilename(const std::string&, GameEngine::VerticalLayout&, std::function<void()>);

private:
    GameEngine::GuiManager& guiManager_;
    GameEngine::GuiElementFactory& guiFactory_;
    std::string font_;
    GameEngine::GuiTextElement* seletedFileText_;
};
}  // namespace Editor
