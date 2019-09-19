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
const float CURRENT_DIR_PERCENT_SIZE = 0.1f;
const float FILE_LIST_PERCENT_SIZE = 0.7f;
const float SELECTED_FILE_PERCENT_SIZE = 0.1f;
const float BUTTON_ACTIONS_PERCENT_SIZE = 0.1f;
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

    CreateWindowWithLayout(vec2(0), vec2(0.6, 0.4));
    CreateCurrentDirBar();
    CreateFileListLayout();
    CreateSeletedFileText();
    CreateActionButtons();
    FillFileList(dir);
}

void FileExplorer::CreateWindowWithLayout(const vec2 &position, const vec2 &size)
{
    auto window = guiFactory_.CreateGuiWindow(position, size);
    window->SetZPosition(-20.f);

    auto layout   = guiFactory_.CreateVerticalLayout();
    windowLayout_ = layout.get();
    windowLayout_->SetAlgin(GameEngine::VerticalLayout::Algin::CENTER);
    windowLayout_->SetScale(size);
    window->AddChild(std::move(layout));

    window_ = window.get();
    guiManager_.Add(std::move(window));
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

    DEBUG_LOG("windowLayout_ position : " + std::to_string(windowLayout_->GetPosition()));
    DEBUG_LOG("window position : " + std::to_string(window_->GetPosition()));

    DEBUG_LOG("windowLayout_ scale : " + std::to_string(windowLayout_->GetScale()));
    DEBUG_LOG("window scale : " + std::to_string(window_->GetScale()));

    DEBUG_LOG("fileLayout_ position : " + std::to_string(fileLayout_->GetPosition()));
    DEBUG_LOG("seletedFileText_ position : " + std::to_string(seletedFileText_->GetPosition()));

    DEBUG_LOG("fileLayout_ scale : " + std::to_string(fileLayout_->GetScale()));
    DEBUG_LOG("seletedFileText_ scale : " + std::to_string(seletedFileText_->GetScale()));
}

void FileExplorer::AddRefillTask(const std::string &dir)
{
    guiManager_.AddTask([this, dir]() {
        fileLayout_->RemoveAll();
        fileLayout_->ResetView();
        FillFileList(dir);
    });
}

void FileExplorer::CreateButtonWithFilename(const std::string &filename, GameEngine::ActionFunction onClick)
{
    auto button = guiFactory_.CreateGuiButton(filename, onClick);
    button->SetZPosition(-1.f);
    fileLayout_->AddChild(std::move(button));
}
void FileExplorer::CreateSeletedFileText()
{
    auto seletedFileText = guiFactory_.CreateEditBox();
    seletedFileText_     = seletedFileText.get();
    seletedFileText_->SetTextColor(vec3(0.2));
    seletedFileText_->SetScale(vec2(window_->GetScale().x, window_->GetScale().y * SELECTED_FILE_PERCENT_SIZE));
    seletedFileText_->SetZPosition(-1.f);
    windowLayout_->AddChild(std::move(seletedFileText));
}
void FileExplorer::CreateActionButtons()
{
    auto okButton = guiFactory_.CreateGuiButton("ok", [this](auto &) {
        auto filename = currentDir_->GetTextString() + "/" + seletedFileText_->GetTextString();
        if (onChoose_(filename))
        {
            window_->MarkToRemove();
        }
        else
        {
            guiFactory_.CreateMessageBox("Error", "File " + filename + " not supported.");
        }
    });

    okButton->SetZPosition(-1.f);
    okButton->GetText()->SetColor(vec3(0.8f));
    windowLayout_->AddChild(std::move(okButton));
}
void FileExplorer::CreateCurrentDirBar()
{
    if (not window_)
        return;

    auto currentDirBar = guiFactory_.CreateEditBox();
    currentDir_ = currentDirBar.get();
    currentDir_->SetTextColor(vec3(0.2));
    currentDir_->SetScale(vec2(window_->GetScale().x, window_->GetScale().y * CURRENT_DIR_PERCENT_SIZE));
    currentDir_->SetZPosition(-1.f);
    currentDir_->SetOnEnterAction([this](const std::string &text) {
        if (Utils::DirectoryExist(text))
        {
            AddRefillTask(text);
        }
    });
    windowLayout_->AddChild(std::move(currentDirBar));
}
void FileExplorer::CreateFileListLayout()
{
    auto layout = guiFactory_.CreateVerticalLayout();
    fileLayout_ = layout.get();

    fileLayout_->SetAlgin(GameEngine::VerticalLayout::Algin::LEFT);
    fileLayout_->SetScale(vec2(window_->GetScale().x, window_->GetScale().y * FILE_LIST_PERCENT_SIZE));
    windowLayout_->AddChild(std::move(layout));
}
}  // namespace Editor
