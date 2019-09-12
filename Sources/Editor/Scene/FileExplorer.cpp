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
    onChoose_ = onChoose;

    const vec2 position(0, 0);
    const vec2 windowScale(0.6, 0.4);

    auto window = guiFactory_.CreateGuiWindow(position, windowScale);
    window_     = window.get();

    window_->SetZPosition(-20.f);

    auto layoutuptr = guiFactory_.CreateVerticalLayout();
    layout_         = layoutuptr.get();

    layout_->SetAlgin(GameEngine::VerticalLayout::Algin::LEFT);
    layout_->SetPostion(position + vec2(0, 0.05));
    layout_->SetScale(vec2(windowScale.x, 0.3));

    if (not layout_ or not window_)
        return;

    auto okButton = guiFactory_.CreateGuiButton("ok", [onChoose, this](auto &) {
        auto filename = currentDir_->GetTextString() + "/" + seletedFileText_->GetTextString();
        if (onChoose(filename))
        {
            window_->MarkToRemove();
        }
        else
        {
            guiFactory_.CreateMessageBox("Error", "File " + filename + " not supported.");
        }
    });

    okButton->SetPostion(vec2(0, -windowScale.y + okButton->GetScale().y));
    okButton->SetZPosition(-1.f);
    okButton->GetText()->SetColor(vec3(0.8f));

    auto seletedFileText = guiFactory_.CreateEditBox();
    seletedFileText_     = seletedFileText.get();
    seletedFileText_->SetTextColor(vec3(0.2));
    seletedFileText_->SetScale(vec2(windowScale.x, 0.04));
    seletedFileText_->SetZPosition(-1.f);
    seletedFileText_->SetPostion(vec2(0, -windowScale.y + (2.f * okButton->GetScale().y) + seletedFileText_->GetScale().y));

    auto currentDir = guiFactory_.CreateEditBox();
    currentDir_     = currentDir.get();
    currentDir_->SetTextColor(vec3(0.2));
    currentDir_->SetScale(vec2(windowScale.x, 0.04));
    currentDir_->SetZPosition(-1.f);
    currentDir_->SetPostion(vec2(0, windowScale.y + seletedFileText_->GetScale().y));
    currentDir_->SetOnEnterAction([this](const std::string &text) {
        if (Utils::DirectoryExist(text))
        {
            AddRefillTask(text);
        }
    });

    window_->AddChild(std::move(seletedFileText));
    window_->AddChild(std::move(currentDir));
    window_->AddChild(std::move(okButton));
    window_->AddChild(std::move(layoutuptr));

    guiManager_.Add(std::move(window));

    FillFileList(dir);
}

void FileExplorer::FillFileList(const std::string &dir)
{
    currentDir_->SetText(dir);

    auto onClickRoot = [this](auto &) {
        const std::string rootDir{"/"};
        AddRefillTask(rootDir);
    };

    CreateButtonWithFilename(".", onClickRoot);

    auto parentDir = Utils::GetParent(dir);
    if (not parentDir.empty())
    {
        auto onClick = [this, parentDir](auto &) { AddRefillTask(parentDir); };
        CreateButtonWithFilename("..", onClick);
    }

    auto filesInDir = Utils::GetFilesInDirectory(dir);
    for (const auto &file : filesInDir)
    {
        DEBUG_LOG(file.name);

        if (file.name.empty())
            continue;

        switch (file.type)
        {
            case Utils::File::Type::RegularFile:
            {
                auto rawFileName = Utils::GetFilenameWithExtension(file.name);
                auto onClick     = [rawFileName, this](auto &) { seletedFileText_->SetText(rawFileName); };
                CreateButtonWithFilename(rawFileName, onClick);
            }
            break;
            case Utils::File::Type::Directory:
            {
                auto onClick = [this, file](auto &) { AddRefillTask(file.name); };
                CreateButtonWithFilename(Utils::GetFilenameWithExtension(file.name) + "/", onClick);
            }
            break;
            case Utils::File::Type::Other:
                break;
        }
    }
}

void FileExplorer::AddRefillTask(const std::string &dir)
{
    guiManager_.AddTask([this, dir]() {
        layout_->RemoveAll();
        layout_->ResetView();
        FillFileList(dir);
    });
}

void FileExplorer::CreateButtonWithFilename(const std::string &filename, GameEngine::ActionFunction onClick)
{
    auto button = guiFactory_.CreateGuiButton(filename, onClick);
    button->SetZPosition(-1.f);
    layout_->AddChild(std::move(button));
}
}  // namespace Editor
