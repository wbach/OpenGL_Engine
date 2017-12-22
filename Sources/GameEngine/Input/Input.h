#pragma once
#include "KeyCodes.h"
#include "GameActions.h"
#include "Types.h"
#include <list>

class CApi;

class CInput 
{
public:
	CInput();
	void SetDefaultKeys();
	virtual bool GetKeyDown(KeyCodes::Type i);
	virtual bool GetKeyUp(KeyCodes::Type  i);
	virtual bool GetKey(KeyCodes::Type i) = 0;

	virtual bool GetMouseKeyDown(int key) = 0;
	virtual bool GetMouseKeyUp(int key) = 0;
	virtual bool GetMouseKey(int key) = 0;
	virtual vec2 CalcualteMouseMove() = 0;
	virtual vec2 GetMousePosition() = 0;

	virtual void SetCursorPosition(int x, int y) = 0;
	virtual void SetKeyToBuffer(int key, bool value) = 0;
	virtual void ClearKeyBuffer() = 0;

	void CheckReleasedKeys();

public:
    KeyCodes::Type keyGameActions[GameActions::COUNT];
    CApi* api;

private:
    std::list<std::pair<KeyCodes::Type, bool>> pressedKeys;
};
