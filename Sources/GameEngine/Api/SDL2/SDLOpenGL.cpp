#include "SDLOpenGL.h"
#include "InputSDL.h"
#include "../Api.hpp"
#include "../../Input/InputManager.h"
#include "Logger/Log.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
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
	CSdlOpenGlApi::~CSdlOpenGlApi()
	{
		Log("CSdlOpenGlApi::~CSdlOpenGlApi() DeleteContext");
		SDL_GL_DeleteContext(glContext);
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		SDL_Quit();
	}

	void CSdlOpenGlApi::CreateOpenGLWindow(const std::string& window_name, const int& width, const int& height, bool full_screen)
	{
		SDL_Init(SDL_INIT_VIDEO);

		auto flags = CreateWindowFlags();
		CreateSDLWindow(window_name, width, height, flags);

		CreateGLContext();

		if (full_screen)
			SetFullScreen(true);

		InitGlew();
		PrintOpenGLInfo();
	}

	void CSdlOpenGlApi::UpdateWindow()
	{
		SDL_GL_SwapWindow(window);
	}
	void CSdlOpenGlApi::SetFullScreen(bool full_screen)
	{
		if (full_screen)
			SDL_SetWindowFullscreen(window, SDL_TRUE);
		else
			SDL_SetWindowFullscreen(window, SDL_FALSE);
	}

	bool CSdlOpenGlApi::CheckActiveWindow()
	{
		return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
	}

	void CSdlOpenGlApi::ShowCursor(bool show)
	{
		SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
	}

	std::shared_ptr<InputManager> CSdlOpenGlApi::CreateInput()
	{
		auto input = std::make_shared<InputSDL>(window);
		addKeyEvent_ = std::bind(&InputSDL::AddKeyEvent, input.get(), std::placeholders::_1, std::placeholders::_2);
		return input;
	}

	double CSdlOpenGlApi::GetTime()
	{
		return static_cast<double>(SDL_GetTicks() / 1000.0);
	}

	void CSdlOpenGlApi::SetCursorPosition(int x, int y)
	{
		SDL_WarpMouseInWindow(window, x, y);
	}

	uint32 CSdlOpenGlApi::CreateWindowFlags() const
	{
		uint32 flags = SDL_WINDOW_OPENGL;
#ifdef EDITOR
		flags |= SDL_WINDOW_BORDERLESS;
#endif
		return flags;
}

	void CSdlOpenGlApi::CreateSDLWindow(const std::string & window_name, const int & width, const int & height, uint32 flags)
	{
		if (window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags))
			return;

		Log("[Error] SDL_CreateWindow error.");
		exit(-1);
	}

	void CSdlOpenGlApi::CreateGLContext()
	{
		if (glContext = SDL_GL_CreateContext(window))
			return;

		Log("[Error] SDL_GL_CreateContext error.");
		exit(-1);
	}

	void CSdlOpenGlApi::InitGlew()
	{
		GLint glew_init_result = glewInit();
		if (glew_init_result == GLEW_OK)
			return;

		std::string err(reinterpret_cast<char const *>(glewGetErrorString(glew_init_result)));
		Log("[Error] Glew init error : " + err);
		exit(-1);
	}

	void CSdlOpenGlApi::PrintOpenGLInfo()
	{
		std::string ver(reinterpret_cast<char const *>(glGetString(GL_VERSION)));
		Log("GL version: " + ver);

		std::string glslver(reinterpret_cast<char const *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
		Log("GLSL version: " + glslver);

		GLint MaxPatchVertices = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
		Log("Max supported patch vertices :" + std::to_string(MaxPatchVertices));
		glPatchParameteri(GL_PATCH_VERTICES, 3);
	}

	void CSdlOpenGlApi::BeginFrame()
	{
		startTime = SDL_GetTicks();
	}

	void CSdlOpenGlApi::LockFps(float fps)
	{
		// Now Lock in Time Measurer function to delete
		// auto delay = static_cast<Uint32>(1000.0f / fps);
		// auto duration = SDL_GetTicks() - startTime;

		// if (delay > duration)
		// 	SDL_Delay(delay - duration);
	}

	ApiMessages::Type CSdlOpenGlApi::PeekMessages()
	{
		BeginFrame();
		while (SDL_PollEvent(&event))
		{
			if (ProcessSdlEvent() == ApiMessages::QUIT)
				return ApiMessages::QUIT;
		}

		return ApiMessages::NONE;
	}

	ApiMessages::Type CSdlOpenGlApi::ProcessSdlEvent() const
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return ApiMessages::QUIT;
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
		return ApiMessages::NONE;
	}

	void CSdlOpenGlApi::ProccesSdlKeyDown(uint32 type) const
	{
		if (addKeyEvent_ == nullptr)
			return;

		if (event.key.repeat != 0)
			return;

		auto key = event.key.keysym.sym;
		auto scanCode = SDL_GetScancodeFromKey(key);

		addKeyEvent_(type, scanCode);
	}
} // GameEngine