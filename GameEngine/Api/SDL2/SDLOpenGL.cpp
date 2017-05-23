#include "SDLOpenGL.h"
#include "../../Debug_/Log.h"
#include "../../Input/Input.h"
#include "../Api.hpp"
#include "InputSDL.h"
#include <GL/glew.h>

CSdlOpenGlApi::~CSdlOpenGlApi()
{
    SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

void CSdlOpenGlApi::CreateOpenGLWindow(const std::string& window_name, const int& width, const int& height, bool full_screen)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	uint flags = SDL_WINDOW_OPENGL;
#ifdef EDITOR
	flags |= SDL_WINDOW_BORDERLESS;
#endif
    if (!(window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags)))
	{
		Log("[Error] SDL_CreateWindow error.");
        exit(0);
		return;
	}
    if (!(glContext = SDL_GL_CreateContext(window)))
	{
		Log("[Error] SDL_GL_CreateContext error.");
        exit(0);
		return;
	}
	if (full_screen)
		SetFullScreen(true);

	GLint glew_init_result = glewInit();
	if (glew_init_result != GLEW_OK)
	{
		std::string err(reinterpret_cast< char const * >(glewGetErrorString(glew_init_result)));
		Log("[Error] Glew init error : " + err);
        exit(0);
		return;
	}
	std::string ver(reinterpret_cast< char const * >(glGetString(GL_VERSION)));
	Log("GL version: " + ver);

	std::string glslver(reinterpret_cast< char const * >(glGetString(GL_SHADING_LANGUAGE_VERSION)));
	Log("GLSL version: " + glslver);

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	Log("Max supported patch vertices :" +  std::to_string(MaxPatchVertices));
	glPatchParameteri(GL_PATCH_VERTICES, 3);
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
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CSdlOpenGlApi::ShowCursor(bool show)
{
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void CSdlOpenGlApi::SetInput(std::unique_ptr<CInput>& input)
{
    input = std::make_unique<CInputSDL>(window);
}

double CSdlOpenGlApi::GetTime()
{
	return  static_cast<double>(SDL_GetTicks() / 1000.0);
}

void CSdlOpenGlApi::SetCursorPosition(int x, int y)
{
    SDL_WarpMouseInWindow(window, x, y);
}

void CSdlOpenGlApi::BeginFrame()
{
    startTime = SDL_GetTicks();
}

void CSdlOpenGlApi::LockFps(float fps)
{
    if (static_cast<Uint32>(1000.0f / fps ) > SDL_GetTicks() - startTime)  SDL_Delay(static_cast<Uint32>(1000.0f / fps ) - (SDL_GetTicks() - startTime));
}

ApiMessages::Type CSdlOpenGlApi::PeekMessages()
{	
	BeginFrame();
    while (SDL_PollEvent(&event))
	{
        switch (event.type)
		{
			case SDL_QUIT: return ApiMessages::QUIT;
			case SDL_MOUSEBUTTONDOWN:

				break;
			case SDL_KEYDOWN:

			//	m_Event.key.keysym.unicode;

                switch (event.key.keysym.sym)
				{
				case SDLK_F9: break; // m_DisplayManager.SetFullScreen(); 
				case SDLK_ESCAPE: return ApiMessages::QUIT;
				}
				break;
			case SDL_FINGERDOWN:
			{

			}
		}
	}
	return ApiMessages::NONE;
}
