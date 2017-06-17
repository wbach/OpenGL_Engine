#pragma once
#include "../../Input/Input.h"
#include "Types.h"

struct SDL_Window;

class CInputSDL : public CInput
{
public:
	CInputSDL(SDL_Window* sdl_window);
	// Keyboard
	virtual bool GetKeyUp(KeyCodes::Type  i) override;
	virtual bool GetKey(KeyCodes::Type  i) override;
	virtual bool GetKeyDown(KeyCodes::Type  i) override;
	
	//Mouse
	virtual bool GetMouseKeyDown(int key) override;
	virtual bool GetMouseKeyUp(int key) override;
	virtual bool GetMouseKey(int key) override;
	virtual vec2 CalcualteMouseMove() override;
	virtual vec2 GetMousePosition() override;

	//not use in SDL
	virtual void SetKeyToBuffer(int key, bool value) override;
	virtual void ClearKeyBuffer() override;
	virtual void SetCursorPosition(int x, int y) override;
private:
	int KeyToSDL(int i);

    SDL_Window* sdlWindow;
};
