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
    font_ = EngineConf_GetFullDataPathAddToRequierd("GUI/monaco.ttf");
}

void FileExplorer::Start(const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    const vec2 position(0, 0);
    const vec2 scale(0.2, 0.4);
    auto window = guiFactory_.CreateGuiWindow("FileExplorer_Window", position, scale, "GUI/darkGrayButton.png");
    auto layout = guiFactory_.CreateVerticalLayout("FileExplorer_layout");
    layout->SetAlgin(GameEngine::VerticalLayout::Algin::LEFT);
    layout->SetPostion(position);
    layout->SetScale(scale);

    if (not layout or not window)
        return;

    FillFileList(*layout, dir, onChoose);

    window->AddChild(layout);
}

void FileExplorer::FillFileList(GameEngine::VerticalLayout &layout, const std::string &dir,
                                std::function<void(const std::string &)> onChoose)
{
    auto filesInDir = Utils::GetFilesInDirectory(dir);
    for (const auto &file : filesInDir)
    {
        if (file.name.empty())
            continue;

        switch (file.type)
        {
            case Utils::File::Type::RegularFile:
            {
                auto onClick = [file, onChoose]() { onChoose(file.name); };
                CreateButtonWithFilename(file.name, layout, onClick);
            }
            break;
            case Utils::File::Type::Directory:
            {
                auto onClick = [this, file, &layout, onChoose]() {
                    layout.RemoveAll();
                    FillFileList(layout, file.name, onChoose);
                };
                CreateButtonWithFilename(file.name + "/", layout, onClick);
            }
            break;
            case Utils::File::Type::Other:
                break;
        }
    }
}

void FileExplorer::CreateButtonWithFilename(const std::string &filename, GameEngine::VerticalLayout &layout,
                                            std::function<void()> onClick)
{
    const std::string constLabel = "FileExplorerRegularFile" + filename;

    auto button = guiFactory_.CreateGuiButton(constLabel + "Button", onClick);

    auto text = guiFactory_.CreateGuiText(constLabel + "ButtonText" + filename, font_, filename, 32, 0);
    if (not text)
    {
        DEBUG_LOG("text create error");
        return;
    }
    text->SetColor(vec3(1, 1, 1));
    button->SetScale(text->GetScale());
    button->SetText(text);

    auto buttonTexture =
        guiFactory_.CreateGuiTexture(constLabel + "Button" + "buttonTexture", "GUI/darkGrayButton.png");
    auto hoverButtonTexture =
        guiFactory_.CreateGuiTexture(constLabel + "Button" + "hoverButtonTexture", "GUI/darkGrayButtonHover.png");
    auto activeButtonTexture =
        guiFactory_.CreateGuiTexture(constLabel + "Button" + "activeButtonTexture", "GUI/darkGrayButtonActive.png");

    if (buttonTexture)
        button->SetBackgroundTexture(buttonTexture);
    if (hoverButtonTexture)
        button->SetOnHoverTexture(hoverButtonTexture);
    if (activeButtonTexture)
        button->SetOnActiveTexture(activeButtonTexture);

    layout.AddChild(button);
}

}  // namespace Editor
