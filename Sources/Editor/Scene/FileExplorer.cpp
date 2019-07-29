#include "FileExplorer.h"
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Utils.h>
#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace Editor
{
const std::string font = "GUI/monaco.ttf";

FileExplorer::FileExplorer(GameEngine::GuiManager &manager, GameEngine::GuiElementFactory &factory)
    : guiManager_(manager)
    , guiFactory_(factory)
{
}

void FileExplorer::Start(const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    auto window = guiFactory_.CreateGuiWindow("FileExplorer_Window", vec2(0, 0), vec2(200, 400), "FileExplorer");
    auto layout = guiFactory_.CreateVerticalLayout("FileExplorer_layout");

    if (not layout or not window)
        return;

    FillFileList(*layout, dir, onChoose);

    window->AddChild(layout);
    onChoose_ = onChoose;
}

void FileExplorer::FillFileList(GameEngine::VerticalLayout &layout, const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    auto filesInDir = Utils::GetFilesInDirectory(dir);
    for (const auto &file : filesInDir)
    {
        const auto &filename = Utils::GetFilename(file.name);

        switch (file.type)
        {
            case Utils::File::Type::RegularFile:
            {
                auto onClick = [filename, onChoose]() { onChoose(filename); };

                auto button = guiFactory_.CreateGuiButton("FileExplorer_RegularFile_button_" + filename, onClick);
                auto text   = guiFactory_.CreateGuiText("FileExplorer_RegularFile_button_text_" + filename, font, filename, 16, 0);
                button->SetText(text);
                layout.AddChild(button);
            }
            break;
            case Utils::File::Type::Directory:
            {
                auto onClick = [this, filename, &layout, onChoose]() {
                    layout.RemoveAll();
                    FillFileList(layout, filename, onChoose);
                };

                auto button = guiFactory_.CreateGuiButton("FileExplorer_Directory_button_" + filename, onClick);
                auto text   = guiFactory_.CreateGuiText("FileExplorer_Directory_button_text_" + filename, font, filename, 16, 0);
                button->SetText(text);
                layout.AddChild(button);
            }
            break;
            case Utils::File::Type::Other:
                break;
        }
    }
}

}  // namespace Editor
