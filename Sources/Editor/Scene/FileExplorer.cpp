#include "FileExplorer.h"
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Utils.h>
#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace Editor
{
FileExplorer::FileExplorer(GameEngine::GuiManager &manager, GameEngine::GuiElementFactory &factory)
    : guiManager_(manager)
    , guiFactory_(factory)
{
}

void FileExplorer::Start(const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    const vec2 position(0, 0);
    const vec2 scale(0.2, 0.4);
    auto window = guiFactory_.CreateGuiWindow("FileExplorer_Window", position, scale, "GUI/darkGrayButton.png");
    auto layout = guiFactory_.CreateVerticalLayout("FileExplorer_layout");
    layout->SetPostion(position);
    layout->SetScale(scale);

    if (not layout or not window)
        return;

    FillFileList(*layout, dir, onChoose);

    window->AddChild(layout);
    onChoose_ = onChoose;
}

void FileExplorer::FillFileList(GameEngine::VerticalLayout &layout, const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    const std::string font = EngineConf_GetFullDataPathAddToRequierd("GUI/monaco.ttf");

    auto filesInDir = Utils::GetFilesInDirectory(dir);
    for (const auto &file : filesInDir)
    {
        const auto &filename = file.name;
        if (filename.empty())
            continue;

        switch (file.type)
        {
            case Utils::File::Type::RegularFile:
            {
                auto onClick = [filename, onChoose]() { onChoose(filename); };

                const std::string constLabel = "FileExplorerRegularFile" + filename;

                auto button = guiFactory_.CreateGuiButton(constLabel + "Button", onClick);
                auto text   = guiFactory_.CreateGuiText(constLabel + "ButtonText" + filename, font, filename, 32, 0);
                if (not text)
                {
                    DEBUG_LOG("text create error");
                    continue;
                }
                text->SetColor(vec3(1, 1, 1));
                button->SetScale(text->GetScale());
                button->SetText(text);

                auto buttonTexture       = guiFactory_.CreateGuiTexture(constLabel + "Button" + "buttonTexture", "GUI/darkGrayButton.png");
                auto hoverButtonTexture  = guiFactory_.CreateGuiTexture(constLabel + "Button" + "hoverButtonTexture", "GUI/darkGrayButtonHover.png");
                auto activeButtonTexture = guiFactory_.CreateGuiTexture(constLabel + "Button" + "activeButtonTexture", "GUI/darkGrayButtonActive.png");

                if (buttonTexture)
                    button->SetBackgroundTexture(buttonTexture);
                if (hoverButtonTexture)
                    button->SetOnHoverTexture(hoverButtonTexture);
                if (activeButtonTexture)
                    button->SetOnActiveTexture(activeButtonTexture);

                layout.AddChild(button);
            }
            break;
            case Utils::File::Type::Directory:
            {
//                auto onClick = [this, filename, &layout, onChoose]() {
//                    layout.RemoveAll();
//                    FillFileList(layout, filename, onChoose);
//                };

//                auto button = guiFactory_.CreateGuiButton("FileExplorer_Directory_button_" + filename, onClick);
//                auto text   = guiFactory_.CreateGuiText("FileExplorer_Directory_button_text_" + filename, font, filename, 16, 0);
//                button->SetText(text);
//                text->SetColor(vec3(1, 1, 1));
//                layout.AddChild(button);
            }
            break;
            case Utils::File::Type::Other:
                break;
        }
    }
}

}  // namespace Editor
