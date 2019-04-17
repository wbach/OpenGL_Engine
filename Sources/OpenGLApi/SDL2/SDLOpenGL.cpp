#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <optional>
#include "InputSDL.h"
#include "Logger/Log.h"
#include "SDLOpenGL.h"

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
struct SdlOpenGlApi::Pimpl
{
    SDL_GLContext glContext;
    SDL_Window* window;
    SDL_Event event;
    std::vector<TTF_Font*> fonts_;
    std::vector<SDL_Surface*> surfaces_;
    std::unordered_map<std::string, uint32> fontNameToIdMap_;

    void Clear()
    {
        for (auto& surface : surfaces_)
        {
            DeleteSurface(surface);
        }
        for (auto& font : fonts_)
        {
            TTF_CloseFont(font);
        }
    }
    void DeleteSurface(SDL_Surface*& surface)
    {
        if (surface)
        {
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
    }
};

SdlOpenGlApi::SdlOpenGlApi()
{
    impl_ = std::make_unique<Pimpl>();
}

SdlOpenGlApi::~SdlOpenGlApi()
{
    impl_->Clear();
    TTF_Quit();

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

    if (TTF_Init() == 1)
    {
        ERROR_LOG("Failed to init TTF");
    }
}

void SdlOpenGlApi::CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                                GraphicsApi::WindowType windowType)
{
    auto flags = CreateWindowFlags(windowType);
    CreateSDLWindow(window_name, width, height, flags);
    SDL_GL_SetSwapInterval(0);
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

std::shared_ptr<Input::InputManager> SdlOpenGlApi::CreateInput()
{
    auto input   = std::make_shared<InputSDL>(impl_->window);
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

std::optional<uint32> SdlOpenGlApi::OpenFont(const std::string& filename, uint32 size)
{
    auto fname = filename + std::to_string(size);
    if (impl_->fontNameToIdMap_.count(fname) > 0)
    {
        return impl_->fontNameToIdMap_.at(fname);
    }

    auto font = TTF_OpenFont(filename.c_str(), static_cast<int>(size));

    if (font)
    {
        impl_->fonts_.push_back(font);
        auto id = impl_->fonts_.size();
        impl_->fontNameToIdMap_.insert({fname, id});
        return id;
    }

    ERROR_LOG("Cannot open font : " + filename);
    return {};
}

std::optional<GraphicsApi::Surface> SdlOpenGlApi::RenderFont(uint32 id, const std::string& text, const vec4& color,
                                                             uint32 outline)
{
    auto index = id - 1;
    if (index >= impl_->fonts_.size())
    {
        return {};
    }
    const auto& font = impl_->fonts_[index];

    SDL_Color _color;
    _color.r = static_cast<uint8>(color.x * 255.f);
    _color.g = static_cast<uint8>(color.y * 255.f);
    _color.b = static_cast<uint8>(color.z * 255.f);
    _color.a = 255;
    TTF_SetFontOutline(font, static_cast<int>(outline));
    auto sdlSurface = TTF_RenderText_Blended(font, text.c_str(), _color);
    TTF_SetFontOutline(font, static_cast<int>(outline));
    if (not sdlSurface)
    {
        ERROR_LOG("Cannot make a text texture" + std::string(SDL_GetError()));
        return {};
    }

    std::optional<uint32> surfaceId;

    for (size_t i = 0; i < impl_->surfaces_.size(); ++i)
    {
        if (impl_->surfaces_[i] == nullptr)
        {
            surfaceId           = i;
            impl_->surfaces_[i] = sdlSurface;
        }
    }

    if (not surfaceId)
    {
        impl_->surfaces_.push_back(sdlSurface);
        surfaceId = impl_->surfaces_.size() - 1;
    }
    return GraphicsApi::Surface{*surfaceId, vec2ui(static_cast<uint32>(sdlSurface->w), static_cast<uint32>(sdlSurface->h)), sdlSurface->format->BytesPerPixel,
                                sdlSurface->pixels};
}

void SdlOpenGlApi::DeleteSurface(uint32 surfaceId)
{
    if (surfaceId >= impl_->surfaces_.size())
    {
        return;
    }

    impl_->DeleteSurface(impl_->surfaces_[surfaceId]);
    impl_->surfaces_[surfaceId] = nullptr;
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

    ERROR_LOG("SDL_CreateWindow error.");
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

void SdlOpenGlApi::ProcessSdlEvent() const
{
    switch (impl_->event.type)
    {
        case SDL_QUIT:
            break;
        case SDL_MOUSEBUTTONDOWN:
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
