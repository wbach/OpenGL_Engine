#include "SDLOpenGL.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <optional>
#include "InputSDL.h"
#include "Logger/Log.h"
#include <Utils/Utils.h>

#include <iostream>
/*
SDL_INIT_TIMER	Initializes the timer subsystem.
SDL_INIT_AUDIO	Initializes the audio subsystem.
SDL_INIT_VIDEO	Initializes the video subsystem.
SDL_INIT_CDROM	Initializes the cdrom subsystem.
SDL_INIT_JOYSTICK	Initializes the joystick subsystem.
SDL_INIT_EVERYTHING	Initialize all of the above.
SDL_INIT_NOPARACHUTE	Prevents SDL from catching fatal signals.
SDL_INIT_EVENTTHREAD
*/

namespace OpenGLApi
{
#ifndef USE_GNU
const SDL_MessageBoxButtonData buttons[] = {
    {/* .flags, .buttonid, .text */ 0, 0, "no"}, {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes"}
    //   {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel"},
};
#else
const SDL_MessageBoxButtonData buttons[] = {
    {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes"}, {/* .flags, .buttonid, .text */ 0, 0, "no"}
    //   {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel"},
};
#endif
const SDL_MessageBoxColorScheme colorScheme = {{/* .colors (.r, .g, .b) */
                                                /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                                                {200, 200, 200},
                                                /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                                                {20, 20, 20},
                                                /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                                                {255, 255, 250},
                                                /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                                                {150, 150, 150},
                                                /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                                                {100, 100, 100}}};

struct SdlOpenGlApi::Pimpl
{
    SDL_GLContext glContext;
    SDL_Window* window;
    SDL_Event event;
};

SdlOpenGlApi::SdlOpenGlApi()
{
    impl_ = std::make_unique<Pimpl>();
}

SdlOpenGlApi::~SdlOpenGlApi()
{
    DEBUG_LOG("destructor");
    if (impl_->window)
    {
        SDL_DestroyWindow(impl_->window);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void SdlOpenGlApi::Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    FillDisplayModes();
}

void SdlOpenGlApi::CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                                    GraphicsApi::WindowType windowType)
{
    auto flags = CreateWindowFlags(windowType);
    CreateSDLWindow(window_name, width, height, flags);
    SDL_GL_SetSwapInterval(0);
    windowSize_ = vec2ui(width, height);
    windowSize_ = GetWindowSize();
}

void SdlOpenGlApi::SetWindowSize(const vec2ui& size)
{
    SDL_SetWindowSize(impl_->window, static_cast<int>(size.x), static_cast<int>(size.y));
    SDL_SetWindowPosition(impl_->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
}

vec2ui SdlOpenGlApi::GetWindowSize() const
{
    if (impl_->window)
    {
        vec2i createdSize(0);
        SDL_GetWindowSize(impl_->window, &(createdSize.x), &(createdSize.y));
        return vec2ui{static_cast<uint32>(createdSize.x), static_cast<uint32>(createdSize.y)};
    }
    return windowSize_;
}

void SdlOpenGlApi::CreateContext()
{
    impl_->glContext = SDL_GL_CreateContext(impl_->window);
    if (impl_->glContext)
        return;

    ERROR_LOG("SDL_GL_CreateContext error.");
}

void SdlOpenGlApi::DeleteContext()
{
    DEBUG_LOG("DeleteContext");
    SDL_GL_DeleteContext(impl_->glContext);
}

void SdlOpenGlApi::UpdateWindow()
{
    SDL_GL_SwapWindow(impl_->window);
}
IdType SdlOpenGlApi::SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)> f)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
    auto id = eventSubscribersEventsPool_.getId();
    eventsSubscribers_.insert({id, f});
    return id;
}
void SdlOpenGlApi::UnsubscribeForEvent(IdType id)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
    auto iter = eventsSubscribers_.find(id);
    if (iter != eventsSubscribers_.end())
    {
        eventSubscribersEventsPool_.releaseId(id);
        eventsSubscribers_.erase(iter);
    }
}
void SdlOpenGlApi::SetFullScreen(bool full_screen)
{
    if (full_screen)
        SDL_SetWindowFullscreen(impl_->window, SDL_TRUE);
    else
        SDL_SetWindowFullscreen(impl_->window, SDL_FALSE);
}

bool SdlOpenGlApi::CheckActiveWindow()
{
    return (SDL_GetWindowFlags(impl_->window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void SdlOpenGlApi::ShowCursor(bool show)
{
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

std::unique_ptr<Input::InputManager> SdlOpenGlApi::CreateInput()
{
    auto input   = std::make_unique<InputSDL>(impl_->window);
    addKeyEvent_ = std::bind(&InputSDL::AddKeyEvent, input.get(), std::placeholders::_1, std::placeholders::_2);
    return std::move(input);
}

double SdlOpenGlApi::GetTime()
{
    return static_cast<double>(SDL_GetTicks() / 1000.0);
}

void SdlOpenGlApi::SetCursorPosition(int x, int y)
{
    SDL_WarpMouseInWindow(impl_->window, x, y);
}

void SdlOpenGlApi::ShowMessageBox(const std::string& title, const std::string& message) const
{
    // SDL_MESSAGEBOX_ERROR
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), nullptr);
}

void SdlOpenGlApi::ShowMessageBox(const std::string& title, const std::string& msg,
                                  std::function<void(bool)> selectedFunc) const
{
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        impl_->window,              /* .window, can be null */
        title.c_str(),              /* .title */
        msg.c_str(),                /* .message */
        SDL_arraysize(buttons),     /* .numbuttons */
        buttons,                    /* .buttons */
        nullptr                     //&colorScheme  /* .colorScheme */
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
    {
        ERROR_LOG("error displaying message box");
        return;
    }

    selectedFunc(buttonid == 1);
}

const std::vector<GraphicsApi::DisplayMode>& SdlOpenGlApi::GetDisplayModes() const
{
    return displayModes_;
}

void SdlOpenGlApi::FillDisplayModes()
{
    auto displayCount = SDL_GetNumVideoDisplays();

    for (int displayIndex = 0; displayIndex < displayCount; ++displayIndex)
    {
        SDL_DisplayMode mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
        for (int modeIndex = 0; modeIndex < SDL_GetNumDisplayModes(displayIndex); ++modeIndex)
        {
            if (SDL_GetDisplayMode(displayIndex, modeIndex, &mode) != 0)
            {
                ERROR_LOG("SDL_GetDisplayMode failed: " + SDL_GetError());
                continue;
            }
            displayModes_.push_back({mode.w, mode.h, mode.refresh_rate, displayIndex});
        }
    }
}

uint32 SdlOpenGlApi::CreateWindowFlags(GraphicsApi::WindowType type) const
{
    uint32 flags = SDL_WINDOW_OPENGL;
    switch (type)
    {
        case GraphicsApi::WindowType::WINDOW_BORDERLESS:
            flags |= SDL_WINDOW_BORDERLESS;
            break;
        case GraphicsApi::WindowType::FULL_SCREEN:
            flags |= SDL_WINDOW_FULLSCREEN;
            break;
        case GraphicsApi::WindowType::WINDOW:
            break;
    }
    return flags;
}

void SdlOpenGlApi::CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags)
{
    impl_->window =
        SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

    if (impl_->window)
        return;

    ERROR_LOG("SDL_CreateWindow error. " + SDL_GetError());
    exit(-1);
}

void SdlOpenGlApi::BeginFrame()
{
    startTime = SDL_GetTicks();
}

void SdlOpenGlApi::ProcessEvents()
{
    BeginFrame();
    while (SDL_PollEvent(&impl_->event))
    {
        ProcessSdlEvent();
    }
}

void SdlOpenGlApi::ProcessSdlEvent()
{
    switch (impl_->event.type)
    {
        case SDL_QUIT:
        {
            std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
            for (const auto& [_, subscriber] : eventsSubscribers_)
            {
                subscriber(GraphicsApi::QuitEvent{});
            }
        }
        break;
        case SDL_MOUSEBUTTONDOWN:
            addKeyEvent_(SDL_KEYDOWN, impl_->event.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            addKeyEvent_(SDL_KEYUP, impl_->event.button.button);
            break;
        case SDL_MOUSEWHEEL:
            if (impl_->event.wheel.y == -1)
            {
                addKeyEvent_(SDL_KEYUP, SDL_BUTTON_MIDDLE);
            }
            else if (impl_->event.wheel.y == 1)
            {
                addKeyEvent_(SDL_KEYDOWN, SDL_BUTTON_MIDDLE);
            }

            break;
        case SDL_KEYDOWN:
            ProccesSdlKeyDown(SDL_KEYDOWN);
            break;
        case SDL_KEYUP:
            ProccesSdlKeyDown(SDL_KEYUP);
            break;
        case SDL_FINGERDOWN:
            break;
        case SDL_DROPFILE:
        {
            std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
            for (const auto& [_, subscriber] : eventsSubscribers_)
            {
                subscriber(GraphicsApi::DropFileEvent{impl_->event.drop.file});
            }
            DEBUG_LOG("Drop file : " + impl_->event.drop.file);
        }
        break;
    }
}

void SdlOpenGlApi::ProccesSdlKeyDown(uint32 type) const
{
    if (addKeyEvent_ == nullptr)
        return;

    if (impl_->event.key.repeat != 0)
        return;

    auto key      = impl_->event.key.keysym.sym;
    auto scanCode = SDL_GetScancodeFromKey(key);

    addKeyEvent_(type, scanCode);
}
}  // namespace OpenGLApi
