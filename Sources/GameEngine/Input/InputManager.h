#pragma once
#include "Input.h"
#include <memory>

class CInput;

class CInputManager
{
public:	
	CInputManager();
	void GetPressedKeys();
	void CheckReleasedKeys();

	void ClearBuffer();

	KeyCodes::Type GetKeyCode(GameActions::Type action);

	bool GetKeyDown(KeyCodes::Type key);
	bool GetKeyUp(KeyCodes::Type key) ;
	bool GetKey(KeyCodes::Type key);

	std::vector<KeyCodes::Type> GetKey();
	std::vector<KeyCodes::Type> GetKeyUp();
	std::vector<KeyCodes::Type> GetKeyDown();

	
	bool GetKeyDown(GameActions::Type action);
	bool GetKeyUp(GameActions::Type action);
	bool GetKey(GameActions::Type action);

	bool GetMouseKeyDown(KeyCodes::Type key);
	bool GetMouseKeyUp(KeyCodes::Type key);
	bool GetMouseKey(KeyCodes::Type key);
	vec2 GetMousePosition();

	vec2 CalcualteMouseMove();

	std::vector<char> GetCharKey(KeyCodes::Type key);
	std::vector<char> GetCharKey();

public:
    std::unique_ptr<CInput> input;

private:	
    int inputSource = 0;
};
