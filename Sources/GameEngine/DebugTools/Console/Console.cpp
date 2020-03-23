#include "Console.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <algorithm>
#include <fstream>

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
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
const std::string CONSOLE_LAYER_NAME{"consoleLayer"};
auto inputType                = Input::SingleCharType::SMALL;
const float WINDOW_Z_POSITION = -10.f;
}  // namespace
Console::Console(Scene &scene)
    : scene_(scene)
    , window_{nullptr}
    , currentCommand_{nullptr}
    , commandHistoryIndex_{0}
{
    scene_.guiManager_->AddLayer(CONSOLE_LAYER_NAME);
    auto window =
        scene_.guiElementFactory_->CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0, 0.5), vec2(1, 0.5));
    window_ = window.get();
    scene_.guiManager_->Add(CONSOLE_LAYER_NAME, std::move(window));

    if (not window_)
        return;

    window_->Hide();
    window_->SetZPosition(WINDOW_Z_POSITION);

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F2, [this]() {
        window_->Show();

        if (not commands_.empty())
            commandHistoryIndex_ = static_cast<int32>(commands_.size() - 1);

        if (not currentCommand_ or currentCommand_->GetText() != COMMAND_CURRSOR)
            currentCommand_ = AddOrUpdateGuiText("");

        scene_.inputManager_->StashSubscribers();
        SubscribeKeys();
    });

    RegisterActions();

    std::ifstream file(".commandHistory");

    if (file.is_open())
    {
        std::string command;
        while (file >> command)
        {
            auto text = AddOrUpdateGuiText(command);
            text->Hide();
            commands_.push_back(COMMAND_CURRSOR + command);
        }
        file.close();
    }
}

void Console::RegisterActions()
{
    commandsActions_.insert({"prefab", [this](const auto &params) { LoadPrefab(params); }});
    commandsActions_.insert({"pos", [this](const auto &params) { PrintPosition(params); }});
    commandsActions_.insert({"setpos", [this](const auto &params) { SetPosition(params); }});
    commandsActions_.insert({"savescene", [this](const auto &params) { SaveScene(params); }});
    commandsActions_.insert({"loadscene", [this](const auto &params) { LoadScene(params); }});
    commandsActions_.insert({"reloadscene", [this](const auto &params) { ReloadScene(params); }});
    commandsActions_.insert({"lognow", [this](const auto &params) { SetImmeditalyLogs(params); }});
    commandsActions_.insert({"snap", [this](const auto &params) { TakeSnapshoot(params); }});
    commandsActions_.insert({"reloadshaders", [this](const auto &params) { ReloadShaders(params); }});
    commandsActions_.insert({"swapRenderMode", [this](const auto &params) { SwapRenderMode(params); }});
    commandsActions_.insert({"editorinterface", [this](const auto &params) { EnableEditorNetworkInterface(params); }});
    commandsActions_.insert({"help", [this](const auto &params) { Help(params); }});
}

void Console::AddCommand(const std::string &command)
{
    std::string c = command.substr(COMMAND_CURRSOR.size());

    auto iter = std::find(commands_.begin(), commands_.end(), command);
    if (iter == commands_.end())
    {
        commands_.push_back(command);
    }
    else
    {
        auto str = *iter;
        commands_.erase(iter);
        commands_.push_back(str);
    }

    std::ofstream file(".commandHistory");
    for (const auto &com : commands_)
        file << com.substr(COMMAND_CURRSOR.size()) << '\n';
    file.close();

    DEBUG_LOG(c);
    ExecuteComand(c);
}

void Console::ExecuteComand(const std::string &commandWithParams)
{
    auto command = commandWithParams.substr(0, commandWithParams.find_first_of(' '));

    if (commandsActions_.count(command) > 0)
    {
        auto params = GetParams(commandWithParams);
        for (const auto &param : params)
        {
            DEBUG_LOG("Param : " + param);
        }
        commandsActions_.at(command)(params);
    }
    else
    {
        AddOrUpdateGuiText("command not found");
    }
}

GuiTextElement *Console::AddOrUpdateGuiText(const std::string &command)
{
    DEBUG_LOG(command);

    GuiTextElement *result{nullptr};

    if (guiTexts_.size() < MAX_GUI_TEXTS)
    {
        MoveUpTexts();
        auto text = scene_.guiElementFactory_->CreateGuiText(
            EngineConf_GetFullDataPathAddToRequierd("GUI/Ubuntu-M.ttf"), COMMAND_CURRSOR + command, 25, 0);
        text->SetAlgin(GuiTextElement::Algin::LEFT);
        text->SetPostion(DEFAULT_TEXT_POSITION);

        result = text.get();
        guiTexts_.push_back(result);
        window_->AddChild(std::move(text));
    }
    else
    {
        const auto &windowPosition = window_->GetPosition();

        MoveUpTexts();
        result = guiTexts_.front();
        result->SetText(COMMAND_CURRSOR);
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

void Console::LoadPrefab(const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        AddOrUpdateGuiText("Can not load prefab. Params : filename objectName.");
        return;
    }

    const auto &filename = params[0];
    const auto &name     = params[1];

    scene_.LoadPrefab(filename, name);
}

void Console::SetPosition(const std::vector<std::string> &args)
{
    if (args.size() < 3)
    {
        AddOrUpdateGuiText("Not enough arguments");
        return;
    }

    auto gameObject = GetGameObject(args[0]);
    if (gameObject)
    {
        auto rigidbody = gameObject->GetComponent<Components::Rigidbody>();

        if (args[1] == "x")
        {
            try
            {
                auto position = gameObject->worldTransform.GetPosition();
                position.x    = std::stof(args[2]);

                if (rigidbody)
                {
                    rigidbody->SetPosition(position);
                    DEBUG_LOG("Set rigidbody position");
                }
                else
                {
                    gameObject->worldTransform.SetPosition(position);
                    gameObject->worldTransform.TakeSnapShoot();
                    DEBUG_LOG("Set transform position");
                }
            }
            catch (...)
            {
                AddOrUpdateGuiText("exception stof");
            }
        }
        else if (args[1] == "y")
        {
            try
            {
                auto position = gameObject->worldTransform.GetPosition();
                position.y    = std::stof(args[2]);
                if (rigidbody)
                {
                    rigidbody->SetPosition(position);
                    DEBUG_LOG("Set rigidbody position");
                }
                else
                {
                    gameObject->worldTransform.SetPosition(position);
                    gameObject->worldTransform.TakeSnapShoot();
                    DEBUG_LOG("Set transform position");
                }
            }
            catch (...)
            {
                AddOrUpdateGuiText("exception stof");
            }
        }
        else if (args[1] == "z")
        {
            try
            {
                auto position = gameObject->worldTransform.GetPosition();
                position.z    = std::stof(args[2]);

                if (rigidbody)
                {
                    rigidbody->SetPosition(position);
                }
                else
                {
                    gameObject->worldTransform.SetPosition(position);
                    gameObject->worldTransform.TakeSnapShoot();
                }
            }
            catch (...)
            {
                AddOrUpdateGuiText("exception stof");
            }
        }
        else
        {
            try
            {
                vec3 position;
                position.x = std::stof(args[2]);
                position.y = std::stof(args[3]);
                position.z = std::stof(args[4]);

                if (rigidbody)
                {
                    rigidbody->SetPosition(position);
                }
                else
                {
                    gameObject->worldTransform.SetPosition(position);
                    gameObject->worldTransform.TakeSnapShoot();
                }
            }
            catch (...)
            {
                AddOrUpdateGuiText("exception stof");
            }
        }
    }
    else
    {
        AddOrUpdateGuiText("GameObject with name not found.");
    }
}

void Console::PrintPosition(const std::vector<std::string> &args)
{
    if (args.empty())
    {
        AddOrUpdateGuiText("GameObject with name not found.");
        return;
    }

    if (auto gameObject = GetGameObject(args[0]))
    {
        AddOrUpdateGuiText("Position of " + args[0] + " : " + std::to_string(gameObject->worldTransform.GetPosition()));
    }
    else
    {
        AddOrUpdateGuiText(args[0] + " not found");
    }
}

void Console::SaveScene(const std::vector<std::string> &params)
{
    if (params.empty())
        return;

    scene_.SaveToFile(params[0]);
}

void Console::LoadScene(const std::vector<std::string> &params)
{
    if (params.empty())
        return;

    bool useSceneNumber{true};
    for (const auto &c : params[0])
    {
        if (not isdigit(c))
        {
            useSceneNumber = false;
            break;
        }
    }

    if (useSceneNumber)
    {
        try
        {
            SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_ID, static_cast<uint32>(std::stoi(params[0])));
            scene_.addSceneEvent(sceneEvent);
        }
        catch (...)
        {
            AddOrUpdateGuiText("LOAD_SCENE_BY_ID stoi exception");
        }
    }
    else
    {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_NAME, params[0]);
        scene_.addSceneEvent(sceneEvent);
    }
}

void Console::ReloadScene(const std::vector<std::string> &)
{
    SceneEvent sceneEvent(SceneEventType::RELOAD_SCENE, 0);
    scene_.addSceneEvent(sceneEvent);
}

void Console::SetImmeditalyLogs(const std::vector<std::string> &params)
{
    if (params.empty())
        return;

    auto use = Utils::StringToBool(params[0]);

    if (use)
        CLogger::Instance().ImmeditalyLog();
    else
        CLogger::Instance().LazyLog();
}

void Console::TakeSnapshoot(const std::vector<std::string> &params)
{
    std::string path{"../../snapshoot/"};
    if (not params.empty())
    {
        path = params[0];
    }

    auto takeSnapshoot = [this, path]() { scene_.resourceManager_->GetGraphicsApi().TakeSnapshoot(path); };
    scene_.resourceManager_->GetGpuResourceLoader().AddFunctionToCall(takeSnapshoot);
}

void Console::ReloadShaders(const std::vector<std::string> &)
{
    scene_.renderersManager_->ReloadShaders();
}

void Console::SwapRenderMode(const std::vector<std::string> &)
{
    scene_.renderersManager_->SwapLineFaceRender();
}

void Console::EnableEditorNetworkInterface(const std::vector<std::string> &params)
{
    if (params.empty() or params[0] == "on" or params[0] == "true")
    {
        scene_.RunNetworkEditorInterface();
    }
    else
    {
        scene_.StopNetworkEditorInterface();
    }
}

void Console::Help(const std::vector<std::string> &)
{
    DEBUG_LOG("All commands : ");

    for (const auto &command : commandsActions_)
    {
        DEBUG_LOG(command.first);
    }
}

std::vector<std::string> Console::GetParams(const std::string &command)
{
    auto result = Utils::SplitString(command, ' ');
    if (result.size() > 1)
    {
        result.erase(result.begin());
        return result;
    }
    else
    {
        return {};
    }
}

void Console::SubscribeKeys()
{
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::ENTER, [&]() {
        if (not window_->IsShow() or not currentCommand_)
        {
            return;
        }
        commandHistoryIndex_ = static_cast<int32>(commands_.size() - 1);
        AddCommand(currentCommand_->GetText());
        currentCommand_ = AddOrUpdateGuiText("");
    });

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::DARROW, [this]() {
        if (commands_.empty())
            return;

        if (commandHistoryIndex_ < 0)
        {
            commandHistoryIndex_ = static_cast<int>(commands_.size() - 1);
        }
        if (commandHistoryIndex_ >= static_cast<int>(commands_.size()))
        {
            commandHistoryIndex_ = 0;
        }

        currentCommand_->SetText(commands_[static_cast<size_t>(commandHistoryIndex_)]);
        ++commandHistoryIndex_;
    });

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::UARROW, [this]() {
        if (commands_.empty())
            return;

        if (commandHistoryIndex_ < 0)
        {
            commandHistoryIndex_ = static_cast<int>(commands_.size() - 1);
        }
        if (commandHistoryIndex_ >= static_cast<int>(commands_.size()))
        {
            commandHistoryIndex_ = 0;
        }

        currentCommand_->SetText(commands_[static_cast<size_t>(commandHistoryIndex_)]);
        --commandHistoryIndex_;
    });
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::LSHIFT, [&]() { inputType = Input::SingleCharType::BIG; });
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::RSHIFT, [&]() { inputType = Input::SingleCharType::BIG; });
    scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::LSHIFT, [&]() { inputType = Input::SingleCharType::SMALL; });
    scene_.inputManager_->SubscribeOnKeyUp(KeyCodes::RSHIFT, [&]() { inputType = Input::SingleCharType::SMALL; });
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
                auto character = Input::KeyCodeToCharConverter::Convert(key, inputType);
                if (character)
                {
                    currentCommand_->Append(*character);
                }
            }
            break;
        }
    });

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F2, [this]() {
        window_->Hide();

        if (currentCommand_ and currentCommand_->GetText() != COMMAND_CURRSOR)
        {
            currentCommand_->Append(" (not executed)");
        }

        scene_.inputManager_->StashPopSubscribers();
    });

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [this]() {
        window_->Hide();

        if (currentCommand_ and currentCommand_->GetText() != COMMAND_CURRSOR)
        {
            currentCommand_->Append(" (not executed)");
        }

        scene_.inputManager_->StashPopSubscribers();
    });
}

GameObject *Console::GetGameObject(const std::string &name)
{
    return scene_.GetGameObject(name);
}

}  // namespace Debug
}  // namespace GameEngine
