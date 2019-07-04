#include "FileExplorer.h"

namespace Editor
{
FileExplorer::FileExplorer(GameEngine::GuiManager &manager, GameEngine::GuiElementFactory &factory)
    : guiManager_(manager)
    , guiFactory_(factory)
{
}

void FileExplorer::Start(const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    onChoose_ = onChoose;
}

}  // namespace Editor
