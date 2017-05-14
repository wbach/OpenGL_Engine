#pragma once
#include "Input.h"
#include <memory>

class CInput;

class CInputManager
{
public:	
	CInputManager();
	void ClearBuffer();

	KeyCodes::Type GetKeyCode(GameActions::Type action);

	bool GetKeyDown(KeyCodes::Type key);
	bool GetKeyUp(KeyCodes::Type key) ;
	bool GetKey(KeyCodes::Type key);

	void CheckReleasedKeys();
	bool GetKeyDown(GameActions::Type action);
	bool GetKeyUp(GameActions::Type action);
	bool GetKey(GameActions::Type action);

	bool GetMouseKeyDown(int key);
	bool GetMouseKeyUp(int key);
	bool GetMouseKey(int key);
	glm::vec2 GetMousePosition();

	glm::vec2 CalcualteMouseMove();

	std::unique_ptr<CInput> m_Input;
private:
	
	int m_InputSource = 0;
};