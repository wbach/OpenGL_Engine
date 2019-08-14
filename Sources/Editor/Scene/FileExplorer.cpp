#include "FileExplorer.h"
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Utils.h>
#include <Utils/FileSystem/FileSystemUtils.hpp>

namespace Editor
{
namespace
{
const std::string FONT_NAME = "GUI/monaco.ttf";
}  // namespace
FileExplorer::FileExplorer(GameEngine::GuiManager &manager, GameEngine::GuiElementFactory &factory)
    : guiManager_(manager)
    , guiFactory_(factory)
{
    font_ = EngineConf_GetFullDataPathAddToRequierd(FONT_NAME);
}

FileExplorer::~FileExplorer()
{
}

void FileExplorer::Start(const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    const vec2 position(0, 0);
    const vec2 windowScale(0.2, 0.4);
    auto window = guiFactory_.CreateGuiWindow("FileExplorerWindow", position, windowScale, "GUI/darkGrayButton.png");
    window->SetZPosition(-20.f);
    auto layout = guiFactory_.CreateVerticalLayout("FileExplorer_layout");
    layout->SetAlgin(GameEngine::VerticalLayout::Algin::LEFT);
    layout->SetPostion(position + vec2(0, 0.05));
    layout->SetScale(vec2(0.2, 0.3));

    if (not layout or not window)
        return;

    FillFileList(*layout, dir, onChoose);

    auto okButton = guiFactory_.CreateGuiButton("FileExplorerOkButton", [window, onChoose, this]() {
        onChoose(seletedFileText_->GetText());
        window->MarkToRemove();
    });

    auto okText = guiFactory_.CreateGuiText("FileExplorerSelectedFileOkText", font_, "ok", 32, 0);
    okButton->SetText(okText);
    okButton->SetScale(okText->GetScale());

    seletedFileText_ = guiFactory_.CreateGuiText("FileExplorerSelectedFileText", font_, "", 32, 0);
    seletedFileText_->SetColor(vec3(0.2));
    auto seletedFileEditBox = guiFactory_.CreateEditBox("FileExplorerSelectedFileEditBox", seletedFileText_);
    seletedFileEditBox->SetScale(vec2(windowScale.x, 0.04));

    auto editBoxBgTexture = guiFactory_.CreateGuiTexture("FileExplorerSelectedFileEditBoxBg", "GUI/white.png");
    seletedFileEditBox->SetBackgroundTexture(editBoxBgTexture);
    window->AddChild(seletedFileEditBox);
    window->AddChild(okButton);

    seletedFileEditBox->SetPostion(vec2(0, -windowScale.y + (2.f *okButton->GetScale().y) + seletedFileEditBox->GetScale().y));
    okButton->SetPostion(vec2(0, -windowScale.y + okButton->GetScale().y));
    window->AddChild(layout);
}

void FileExplorer::FillFileList(GameEngine::VerticalLayout &layout, const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    auto parentDir = dir.substr(0, dir.find_last_of('/'));

    auto onClick = [this, parentDir, &layout, onChoose]() {
        layout.RemoveAll();
        FillFileList(layout, parentDir, onChoose);
    };
    CreateButtonWithFilename(parentDir, layout, onClick);

    auto filesInDir = Utils::GetFilesInDirectory(dir);
    for (const auto &file : filesInDir)
    {
        if (file.name.empty())
            continue;

        switch (file.type)
        {
            case Utils::File::Type::RegularFile:
            {
                auto onClick = [file, this]() { seletedFileText_->SetText(file.name); };
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

void FileExplorer::CreateButtonWithFilename(const std::string &filename, GameEngine::VerticalLayout &layout, std::function<void()> onClick)
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

}  // namespace Editor
