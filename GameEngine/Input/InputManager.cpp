#include "InputManager.h"

CInputManager::CInputManager()
: input(nullptr)
{
}

void CInputManager::ClearBuffer()
{
    if (input != nullptr)
        input->ClearKeyBuffer();
}

KeyCodes::Type CInputManager::GetKeyCode(GameActions::Type action)
{
    return input->keyGameActions[action];
}

bool CInputManager::GetKeyDown(KeyCodes::Type key)
{
    return input->GetKeyDown(key);
}

bool CInputManager::GetKeyUp(KeyCodes::Type key)
{
	return false;
}

bool CInputManager::GetKey(KeyCodes::Type key)
{
    if (input != nullptr)
        return input->GetKey(key);
	return false;
}

void CInputManager::CheckReleasedKeys()
{
    if (input != nullptr)
        input->CheckReleasedKeys();
}

bool CInputManager::GetKeyDown(GameActions::Type action)
{
    if (input != nullptr)
        return input->GetKeyDown(input->keyGameActions[action]);
	return false;
}

bool CInputManager::GetKey(GameActions::Type action)
{
    if (input != nullptr)
	{
        if (input->keyGameActions[action] == KeyCodes::LMOUSE ||
            input->keyGameActions[action] == KeyCodes::RMOUSE)
            return input->GetMouseKey(input->keyGameActions[action]);

        return input->GetKey(input->keyGameActions[action]);
	}
	return false;
}

bool CInputManager::GetMouseKeyDown(int key)
{
	return false;
}

bool CInputManager::GetMouseKeyUp(int key)
{
	return false;
}

bool CInputManager::GetMouseKey(int key)
{
    if (input != nullptr)
        return input->GetMouseKey(key);
	return false;
}

vec2 CInputManager::GetMousePosition()
{
    if (input != nullptr)
        return input->GetMousePosition();
	return vec2();
}

vec2 CInputManager::CalcualteMouseMove()
{
    if (input != nullptr)
        return input->CalcualteMouseMove();

	return vec2();
}
