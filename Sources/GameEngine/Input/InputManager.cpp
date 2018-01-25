#include "InputManager.h"

// TO DO: issue : keydown when called 2 times or more only one can working

CInputManager::CInputManager()
: input(nullptr)
{
}

void CInputManager::GetPressedKeys()
{
	if (input != nullptr)
		input->GetPressedKeys();
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

bool CInputManager::GetMouseKeyDown(KeyCodes::Type key)
{
	return false;
}

bool CInputManager::GetMouseKeyUp(KeyCodes::Type key)
{
	return false;
}

bool CInputManager::GetMouseKey(KeyCodes::Type key)
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

std::vector<char> CInputManager::GetCharKey(KeyCodes::Type key)
{
	if (input != nullptr)
		return input->GetCharKey(key);
	return std::vector<char>();
}

std::vector<char> CInputManager::GetCharKey()
{
	if (input != nullptr)
		return input->GetCharKey();
	return std::vector<char>();
}

std::vector<KeyCodes::Type> CInputManager::GetKey()
{
	if (input != nullptr)
		return input->GetKey();

	return std::vector<KeyCodes::Type>();
}

std::vector<KeyCodes::Type> CInputManager::GetKeyUp()
{
	if (input != nullptr)
		return input->GetKeyUp();

	return std::vector<KeyCodes::Type>();
}

std::vector<KeyCodes::Type> CInputManager::GetKeyDown()
{
	if (input != nullptr)
		return input->GetKeyDown();

	return std::vector<KeyCodes::Type>();
}