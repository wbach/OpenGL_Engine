#include "SDLOpenGL.h"
#include <SDL2/SDL.h>
#include "GameEngine/Input/InputManager.h"
#include "InputSDL.h"
#include "Logger/Log.h"

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

namespace GameEngine
{
SdlOpenGlApi::~SdlOpenGlApi()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void SdlOpenGlApi::CreateWindow(const std::string& window_name, uint32 width, uint32 height, WindowType windowType)
{
    SDL_Init(SDL_INIT_VIDEO);

    auto flags = CreateWindowFlags(windowType);
    CreateSDLWindow(window_name, width, height, flags);

    SDL_GL_SetSwapInterval(0);
}

void SdlOpenGlApi::CreateContext()
{
    glContext = SDL_GL_CreateContext(window);
    if (glContext)
        return;

    Log("[Error] SDL_GL_CreateContext error.");
}

void SdlOpenGlApi::DeleteContext()
{
    Log("CSdlOpenGlApi::~CSdlOpenGlApi() DeleteContext");
    SDL_GL_DeleteContext(glContext);
}

void SdlOpenGlApi::UpdateWindow()
{
    SDL_GL_SwapWindow(window);
}
void SdlOpenGlApi::SetFullScreen(bool full_screen)
{
    if (full_screen)
        SDL_SetWindowFullscreen(window, SDL_TRUE);
    else
        SDL_SetWindowFullscreen(window, SDL_FALSE);
}

bool SdlOpenGlApi::CheckActiveWindow()
{
    return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void SdlOpenGlApi::ShowCursor(bool show)
{
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

std::shared_ptr<InputManager> SdlOpenGlApi::CreateInput()
{
    auto input   = std::make_shared<InputSDL>(window);
    addKeyEvent_ = std::bind(&InputSDL::AddKeyEvent, input.get(), std::placeholders::_1, std::placeholders::_2);
    return input;
}

double SdlOpenGlApi::GetTime()
{
    return static_cast<double>(SDL_GetTicks() / 1000.0);
}

void SdlOpenGlApi::SetCursorPosition(int x, int y)
{
    SDL_WarpMouseInWindow(window, x, y);
}

uint32 SdlOpenGlApi::CreateWindowFlags(WindowType type) const
{
    uint32 flags = SDL_WINDOW_OPENGL;
    switch (type)
    {
        case WindowType::WINDOW_BORDERLESS:
            flags |= SDL_WINDOW_BORDERLESS;
            break;
        case WindowType::FULL_SCREEN:
            flags |= SDL_WINDOW_FULLSCREEN;
            break;
        case WindowType::WINDOW:
            break;
    }
    return flags;
}

void SdlOpenGlApi::CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags)
{
    window =
        SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

    if (window)
        return;

    Log("[Error] SDL_CreateWindow error.");
    exit(-1);
}

void SdlOpenGlApi::BeginFrame()
{
    startTime = SDL_GetTicks();
}

void SdlOpenGlApi::ProcessEvents()
{
    BeginFrame();
    while (SDL_PollEvent(&event))
    {
        ProcessSdlEvent();
    }
}

void SdlOpenGlApi::ProcessSdlEvent() const
{
    switch (event.type)
    {
        case SDL_QUIT:
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEWHEEL:
            if (event.wheel.y == -1)
            {
                addKeyEvent_(SDL_KEYUP, SDL_BUTTON_MIDDLE);
            }
            else if (event.wheel.y == 1)
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
    }
}

void SdlOpenGlApi::ProccesSdlKeyDown(uint32 type) const
{
    if (addKeyEvent_ == nullptr)
        return;

    if (event.key.repeat != 0)
        return;

    auto key      = event.key.keysym.sym;
    auto scanCode = SDL_GetScancodeFromKey(key);

    addKeyEvent_(type, scanCode);
}
}  // GameEngine
