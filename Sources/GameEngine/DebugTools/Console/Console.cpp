#include "Console.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <algorithm>
#include <fstream>

#include "GameEngine/Components/Physics/Rigidbody.h"
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
const vec2 DEFAULT_TEXT_POSITION(-0.99f, -0.45f);
const std::string COMMAND_CURRSOR{"> "};
const std::string CONSOLE_LAYER_NAME{"consoleLayer"};
Input::SingleCharType inputType{Input::SingleCharType::SMALL};
const float WINDOW_Z_POSITION{-100.f};
}  // namespace

Console::Console(Scene &scene)
    : scene_(scene)
    , window_{nullptr}
    , currentCommand_{nullptr}
    , commandHistoryIndex_{0}
    , keysSubscribtionManager_(*scene_.inputManager_)
{
    PrepareConsoleWindow();
    RegisterActions();
    LoadCommandHistoryFromFile();
}

Console::~Console()
{
    DEBUG_LOG("destructor");
}

void Console::ExecuteCommands()
{
    while (not commandsToExecute_.empty())
    {
        auto command = GetCommandToExecute();
        ExecuteComand(command);
    }
}

void Console::AddAndUpdateHistoryFileIfNeeded(const std::string &command)
{
    auto iter = std::find(commandsHistory_.begin(), commandsHistory_.end(), command);

    if (iter == commandsHistory_.end())
    {
        commandsHistory_.push_back(command);
    }
    else
    {
        // move item to end
        auto str = *iter;
        commandsHistory_.erase(iter);
        commandsHistory_.push_back(str);
    }

    std::ofstream file(".commandHistory");
    for (const auto &com : commandsHistory_)
        file << com << '\n';
    file.close();
}

void Console::RegisterActions()
{
    commandsActions_.insert({"freecam", [this](const auto &params) { SetFreeCamera(params); }});
    commandsActions_.insert({"disablefreecam", [this](const auto &params) { DisableFreeCam(params); }});
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
    commandsActions_.insert({"camera", [this](const auto &params) { CameraInfo(params); }});
    commandsActions_.insert({"exit", [this](const auto &params) { Exit(params); }});
    commandsActions_.insert({"quit", [this](const auto &params) { Exit(params); }});
}

void Console::CameraInfo(const std::vector<std::string> &)
{
    PrintMsgInConsole("Camera info: ");
    PrintMsgInConsole(std::to_string(scene_.camera.GetPosition()));
    PrintMsgInConsole(std::to_string(scene_.camera.GetRotation().GetEulerDegrees().value));
}

void Console::SetFreeCamera(const std::vector<std::string> &)
{
    if (firstPersonCamera_)
        return;

    firstPersonCamera_ = std::make_unique<FirstPersonCamera>(*scene_.inputManager_, *scene_.displayManager_);
    stashedCamera_     = scene_.GetCamera().Get();
    firstPersonCamera_->SetPosition(stashedCamera_->GetPosition());
    firstPersonCamera_->SetRotation(stashedCamera_->GetRotation());
    scene_.SetCamera(*firstPersonCamera_);
}

void Console::DisableFreeCam(const std::vector<std::string> &)
{
    scene_.SetCamera(*stashedCamera_);
    firstPersonCamera_.reset(nullptr);
}

void Console::AddCommand(const std::string &command)
{
    if (not IsKnownCommand(command))
    {
        AddOrUpdateGuiText("command not found");
        return;
    }
    AddCommandToExecute(command);
}

void Console::ExecuteComand(const std::string &commandWithParams)
{
    AddAndUpdateHistoryFileIfNeeded(commandWithParams);
    auto command = GetCommandNameFromString(commandWithParams);
    auto params  = GetParams(commandWithParams);
    commandsActions_.at(command)(params);
}

GuiTextElement *Console::AddOrUpdateGuiText(const std::string &command)
{
    MoveUpTexts();
    GuiTextElement *result{nullptr};

    if (guiTexts_.size() < MAX_GUI_TEXTS)
    {
        auto text = scene_.guiElementFactory_->CreateGuiText("GUI/Ubuntu-M.ttf", COMMAND_CURRSOR + command, 35, 0);
        text->SetAlgin(GuiTextElement::Algin::LEFT);
        result = text.get();
        guiTexts_.push_back(result);
        window_->AddChild(std::move(text));
    }
    else
    {
        result = guiTexts_.front();
        result->SetText(COMMAND_CURRSOR + command);
        guiTexts_.pop_front();
        guiTexts_.push_back(result);
    }

    const auto &windowPosition = window_->GetPosition();
    result->SetPostion(windowPosition + DEFAULT_TEXT_POSITION);
    return result;
}

void Console::PrintMsgInConsole(const std::string &msg)
{
    currentCommand_->SetText(msg);
    currentCommand_ = AddOrUpdateGuiText("");
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

bool Console::IsKnownCommand(const std::string &commandWithParams) const
{
    return commandsActions_.count(GetCommandNameFromString(commandWithParams)) > 0;
}

std::string Console::GetCommandNameFromString(const std::string &commandWithParams) const
{
    return commandWithParams.substr(0, commandWithParams.find_first_of(' '));
}

void Console::AddCommandToExecute(const std::string &command)
{
    std::lock_guard<std::mutex> lk(commandsExecuteMutex_);
    commandsToExecute_.push_back(command);
}

std::string Console::GetCommandToExecute()
{
    std::lock_guard<std::mutex> lk(commandsExecuteMutex_);
    auto result = commandsToExecute_.front();
    commandsToExecute_.pop_front();
    return result;
}

void Console::CloseConsole()
{
    window_->Hide();
    scene_.camera.Unlock();

    if (currentCommand_ and currentCommand_->GetText() != COMMAND_CURRSOR)
    {
        currentCommand_->Append(" (not executed)");
        currentCommand_->Hide();
    }

    // stash pop remove existing subscriptions
    scene_.inputManager_->StashPopSubscribers();
}

void Console::LoadPrefab(const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        PrintMsgInConsole("Can not load prefab. Params : filename objectName.");
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
        PrintMsgInConsole("Not enough arguments");
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
                PrintMsgInConsole("exception stof");
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
                PrintMsgInConsole("exception stof");
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
                PrintMsgInConsole("exception stof");
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
                PrintMsgInConsole("exception stof");
            }
        }
    }
    else
    {
        PrintMsgInConsole("GameObject with name not found.");
    }
}

void Console::PrintPosition(const std::vector<std::string> &args)
{
    if (args.empty())
    {
        PrintMsgInConsole("GameObject with name not found.");
        return;
    }

    if (auto gameObject = GetGameObject(args[0]))
    {
        PrintMsgInConsole("Position of " + args[0] + " : " + std::to_string(gameObject->worldTransform.GetPosition()));
    }
    else
    {
        PrintMsgInConsole(args[0] + " not found");
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
            PrintMsgInConsole("LOAD_SCENE_BY_ID stoi exception");
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
    // need to close console for key subscribtions are stashed when console is running
    // subscribe and unsubsribe

    CloseConsole();

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

void Console::Exit(const std::vector<std::string> &)
{
    scene_.addEngineEvent(EngineEvent::ASK_QUIT);
}

std::vector<std::string> Console::GetParams(const std::string &command) const
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

void Console::LoadCommandHistoryFromFile()
{
    std::ifstream file(".commandHistory");

    if (file.is_open())
    {
        std::string command;
        while (std::getline(file, command))
        {
            auto text = AddOrUpdateGuiText(command);
            text->Hide();
            commandsHistory_.push_back(command);
        }
        file.close();
    }
}

void Console::SubscribeKeys()
{
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::ENTER, [&]() {
        if (not window_->IsShow() or not currentCommand_)
        {
            return;
        }
        commandHistoryIndex_ = static_cast<int32>(commandsHistory_.size() - 1);
        AddCommand(currentCommand_->GetText().substr(COMMAND_CURRSOR.size()));
        currentCommand_ = AddOrUpdateGuiText("");
    });
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::DARROW, [this]() {
        if (commandsHistory_.empty())
            return;

        if (commandHistoryIndex_ < 0)
        {
            commandHistoryIndex_ = static_cast<int>(commandsHistory_.size() - 1);
        }
        if (commandHistoryIndex_ >= static_cast<int>(commandsHistory_.size()))
        {
            commandHistoryIndex_ = 0;
        }

        currentCommand_->SetText(COMMAND_CURRSOR + commandsHistory_[static_cast<size_t>(commandHistoryIndex_)]);
        ++commandHistoryIndex_;
    });

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::UARROW, [this]() {
        if (commandsHistory_.empty())
            return;

        if (commandHistoryIndex_ < 0)
        {
            commandHistoryIndex_ = static_cast<int>(commandsHistory_.size() - 1);
        }
        if (commandHistoryIndex_ >= static_cast<int>(commandsHistory_.size()))
        {
            commandHistoryIndex_ = 0;
        }

        currentCommand_->SetText(COMMAND_CURRSOR + commandsHistory_[static_cast<size_t>(commandHistoryIndex_)]);
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
                if (currentCommand_->GetText().size() > COMMAND_CURRSOR.size())
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

    auto closeConsole = [this]() { scene_.inputManager_->AddEvent([&]() { CloseConsole(); }); };

    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F2, closeConsole);
    scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, closeConsole);
}  // namespace Debug

GameObject *Console::GetGameObject(const std::string &name)
{
    return scene_.GetGameObject(name);
}

void Console::PrepareConsoleWindow()
{
    DEBUG_LOG("");
    scene_.guiManager_->AddLayer(CONSOLE_LAYER_NAME);

    auto window =
        scene_.guiElementFactory_->CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0, 0.5), vec2(1, 0.5));

    window_ = window.get();
    window_->Hide();
    window_->SetZPosition(WINDOW_Z_POSITION);

    scene_.guiManager_->Add(CONSOLE_LAYER_NAME, std::move(window));

    keysSubscribtionManager_ = scene_.inputManager_->SubscribeOnKeyDown(KeyCodes::F2, [this]() {
        window_->Show();
        DEBUG_LOG("f2");
        if (not commandsHistory_.empty())
            commandHistoryIndex_ = static_cast<int32>(commandsHistory_.size() - 1);

        if (not currentCommand_ or currentCommand_->GetText() != COMMAND_CURRSOR)
            currentCommand_ = AddOrUpdateGuiText("");

        currentCommand_->Show();

        scene_.camera.Lock();
        scene_.inputManager_->AddEvent([&]() {
            scene_.inputManager_->StashSubscribers();
            SubscribeKeys();
        });
    });
}
}  // namespace Debug
}  // namespace GameEngine
