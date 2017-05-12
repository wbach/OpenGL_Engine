#pragma once
#include "glm/glm.hpp"
#include "GameActions.h"
#include <list>

namespace KeyCodes
{
	enum Type
	{
		Q = 0x01,
		W, E, R, T, Y, U, I, O, P,
		A, S, D, F, G, H, J, K, L,
		Z, X, C, V, B, N, M,
		LCTRL, ENTER, LMOUSE, RMOUSE, SPACE, ESCAPE,
		LARROW, RARROW, UARROW, DARROW
	};
}

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
	virtual glm::vec2 CalcualteMouseMove() = 0;
	virtual glm::vec2 GetMousePosition() = 0;

	virtual void SetCursorPosition(int x, int y) = 0;
	virtual void SetKeyToBuffer(int key, bool value) = 0;
	virtual void ClearKeyBuffer() = 0;

	void CheckReleasedKeys();
	KeyCodes::Type m_KeyGameActions[GameActions::COUNT];

	CApi* m_Api;

private:
	std::list<std::pair<KeyCodes::Type, bool>> m_PressedKeys;
};