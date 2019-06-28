#include "Console.h"
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Input/KeyCodeToCharConverter.h"

namespace GameEngine
{
namespace Debug
{
namespace
{
const size_t MAX_GUI_TEXTS{20};
const vec2 DEFAULT_TEXT_POSITION(-1.00, -0.45);
const std::string COMMAND_CURRSOR{"> "};
}  // namespace
Console::Console(Scene &scene)
    : scene_(scene)
    , window_{nullptr}
    , currentCommand_{nullptr}
{
    window_ = scene_.guiElementFactory_->CreateGuiWindow("DebugConsoleWindow", vec2(0, 0.5), vec2(1, 0.5), "GUI/darkGrayButton.png");
    if (not window_)
        return;

    window_->Hide();
    window_->SetZPosition(1.6f);

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F2, [&]() {
        if (not window_)
            return;

        if (window_->IsShow())
        {
            window_->Hide();

            if (currentCommand_ and currentCommand_->GetText() != COMMAND_CURRSOR)
            {
                currentCommand_->Append(" (not executed)");
            }
        }
        else
        {
            window_->Show();
            if (not currentCommand_ or currentCommand_->GetText() != COMMAND_CURRSOR)
                currentCommand_ = AddOrUpdateGuiText("");
        }
    });
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::ENTER, [&]() {
        if (not window_->IsShow() or not currentCommand_)
        {
            return;
        }

        AddCommand(currentCommand_->GetText());
        currentCommand_ = AddOrUpdateGuiText("");
    });

    scene_.inputManager_->SubscribeOnAnyKeyPress([this](KeyCodes::Type key) {
        if (not window_->IsShow())
            return;

        switch (key)
        {
            case KeyCodes::SPACE:
                currentCommand_->Append(' ');
                break;

            case KeyCodes::BACKSPACE:
                currentCommand_->Pop();
                break;

            default:
            {
                auto character = Input::KeyCodeToCharConverter::Convert(key, Input::SingleCharType::SMALL);
                if (character)
                {
                    currentCommand_->Append(*character);
                }
            }
            break;
        }
    });

    RegisterActions();
}

void Console::AddCommand(const std::string &command)
{
    DEBUG_LOG(command);
    commands_.push_back(command);
    ExecuteComand(command);
}

void Console::ExecuteComand(const std::string &command)
{
    if (commandsActions_.count(command) > 0)
    {
        auto params = GetParams(command);
        commandsActions_.at(command)(params);
    }
    else
    {
        AddOrUpdateGuiText("command not found");
    }
}

GuiTextElement *Console::AddOrUpdateGuiText(const std::string &command)
{
    GuiTextElement *result{nullptr};

    if (guiTexts_.size() < MAX_GUI_TEXTS)
    {
        MoveUpTexts();
        auto text = scene_.guiElementFactory_->CreateGuiText("DebugConsoleText_" + std::to_string(guiTexts_.size()), EngineConf_GetFullDataPathAddToRequierd("GUI/Ubuntu-M.ttf"), COMMAND_CURRSOR + command, 25, 0);
        result    = text;
        text->SetAlgin(GuiTextElement::Algin::LEFT);
        guiTexts_.push_back(text);
        text->SetPostion(DEFAULT_TEXT_POSITION);

        window_->AddChild(text);
    }
    else
    {
        MoveUpTexts();
        result = guiTexts_.front();
        result->SetText(COMMAND_CURRSOR);
        const auto &windowPosition = window_->GetPosition();
        result->SetPostion(windowPosition + DEFAULT_TEXT_POSITION);
        guiTexts_.pop_front();
        guiTexts_.push_back(result);
    }
    return result;
}

void Console::MoveUpTexts()
{
    for (auto &guiText : guiTexts_)
    {
        auto position = guiText->GetPosition();
        auto scale    = guiText->GetScale();
        position.y += 2.f * scale.y;
        guiText->SetPostion(position);
    }
}

void Console::RegisterActions()
{
    commandsActions_.insert({"prefab", [this](const auto &params) { LoadPrefab(params); }});
}

void Console::LoadPrefab(const std::vector<std::string> &params)
{
    if (params.size() < 4)
    {
        DEBUG_LOG("Can not load prefab. Params : filename objectName");
        return;
    }

    const auto &filename = params[0];
    const auto &name     = params[1];

    scene_.LoadPrefab(filename, name);
}

std::vector<std::string> Console::GetParams(const std::string &command)
{
    auto result = Utils::SplitString(command, ' ');
    if (result.size() > 2)
    {
        result.erase(result.begin());
        result.erase(result.begin());
        return result;
    }
    else
    {
        return {};
    }
}

}  // namespace Debug
}  // namespace GameEngine
