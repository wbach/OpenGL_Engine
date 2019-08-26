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
    window_->MarkToRemove();
}

void FileExplorer::Start(const std::string &dir, std::function<bool(const std::string &)> onChoose)
{
    const vec2 position(0, 0);
    const vec2 windowScale(0.2, 0.4);

    window_ = guiFactory_.CreateGuiWindow(position, windowScale);
    window_->SetZPosition(-20.f);

    auto layout = guiFactory_.CreateVerticalLayout();
    layout->SetAlgin(GameEngine::VerticalLayout::Algin::LEFT);
    layout->SetPostion(position + vec2(0, 0.05));
    layout->SetScale(vec2(0.2, 0.3));

    if (not layout or not window_)
        return;

    FillFileList(layout, dir, onChoose);

    auto okButton = guiFactory_.CreateGuiButton("ok", [onChoose, this]() {
        if (onChoose(seletedFileText_->GetTextString()))
        {
            window_->MarkToRemove();
        }
        else
        {
            guiFactory_.CreateMessageBox("Error", "File not supported.");
        }
    });
    okButton->SetPostion(vec2(0, -windowScale.y + okButton->GetScale().y));
    okButton->SetZPosition(-1.f);
    okButton->GetText()->SetColor(vec3(0.8f));

    seletedFileText_ = guiFactory_.CreateEditBox();
    seletedFileText_->SetTextColor(vec3(0.2));

    seletedFileText_->SetScale(vec2(windowScale.x, 0.04));
    seletedFileText_->SetZPosition(-1.f);
    seletedFileText_->SetPostion(vec2(0, -windowScale.y + (2.f * okButton->GetScale().y) + seletedFileText_->GetScale().y));

    window_->AddChild(seletedFileText_);
    window_->AddChild(okButton);
    window_->AddChild(layout);
}

void FileExplorer::FillFileList(GameEngine::VerticalLayout *layout, const std::string &dir, std::function<void(const std::string &)> onChoose)
{
    auto onClickRoot = [this, layout, onChoose]() {
        layout->RemoveAll();
        layout->ResetView();
        FillFileList(layout, "/", onChoose);
    };
    CreateButtonWithFilename(".", layout, onClickRoot);

    auto parentDir = Utils::GetParent(dir);
    DEBUG_LOG("Parent dir : " + parentDir);

    if (not parentDir.empty())
    {
        auto onClick = [this, parentDir, layout, onChoose]() {
            layout->RemoveAll();
            layout->ResetView();
            FillFileList(layout, parentDir, onChoose);
        };
        CreateButtonWithFilename("..", layout, onClick);
    }

    auto filesInDir = Utils::GetFilesInDirectory(dir);
    for (const auto &file : filesInDir)
    {
        if (file.name.empty())
            continue;

        switch (file.type)
        {
            case Utils::File::Type::RegularFile:
            {
                auto rawFileName = Utils::GetFilenameWithExtension(file.name);
                auto onClick     = [rawFileName, this]() { seletedFileText_->SetText(rawFileName); };
                CreateButtonWithFilename(rawFileName, layout, onClick);
            }
            break;
            case Utils::File::Type::Directory:
            {
                auto onClick = [this, file, layout, onChoose]() {
                    layout->RemoveAll();
                    layout->ResetView();
                    FillFileList(layout, file.name, onChoose);
                };
                CreateButtonWithFilename(Utils::GetFilenameWithExtension(file.name) + "/", layout, onClick);
            }
            break;
            case Utils::File::Type::Other:
                break;
        }
    }
}

void FileExplorer::CreateButtonWithFilename(const std::string &filename, GameEngine::VerticalLayout *layout, std::function<void()> onClick)
{
    auto button = guiFactory_.CreateGuiButton(filename, onClick);
    button->SetZPosition(-1.f);
    layout->AddChild(button);
}
}  // namespace Editor
