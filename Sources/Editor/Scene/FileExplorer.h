#pragma once
#include <GameEngine/Renderers/GUI/GuiElementFactory.h>
#include <GameEngine/Renderers/GUI/GuiManager.h>
#include <functional>
#include <string>

namespace Editor
{
class FileExplorer
{
public:
    FileExplorer(GameEngine::GuiManager&, GameEngine::GuiElementFactory& );
    void Start(const std::string&, std::function<void(const std::string&)>);

private:
    GameEngine::GuiManager& guiManager_;
    GameEngine::GuiElementFactory& guiFactory_;
    std::function<void(const std::string&)> onChoose_;
};
}  // namespace Editor
