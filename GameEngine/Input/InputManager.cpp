#include "InputManager.h"

CInputManager::CInputManager()
: m_Input(nullptr)
{

}

void CInputManager::ClearBuffer()
{
	if (m_Input != nullptr)
	{
		m_Input->ClearKeyBuffer();
	}
}

KeyCodes::Type CInputManager::GetKeyCode(GameActions::Type action)
{
	return m_Input->m_KeyGameActions[action];
}

bool CInputManager::GetKeyDown(KeyCodes::Type key)
{
	return m_Input->GetKeyDown(key);
}

bool CInputManager::GetKeyUp(KeyCodes::Type key)
{
	return false;
}

bool CInputManager::GetKey(KeyCodes::Type key)
{

	if (m_Input != nullptr)
	{
		return m_Input->GetKey(key);
	}
	return false;
}

void CInputManager::CheckReleasedKeys()
{
	if (m_Input != nullptr)
	{
		m_Input->CheckReleasedKeys();
	}
}

bool CInputManager::GetKeyDown(GameActions::Type action)
{
	if (m_Input != nullptr)
	{
		return m_Input->GetKeyDown(m_Input->m_KeyGameActions[action]);
	}
	return false;
}

bool CInputManager::GetKey(GameActions::Type action)
{
	if (m_Input != nullptr)
	{
		if (m_Input->m_KeyGameActions[action] == KeyCodes::LMOUSE ||
			m_Input->m_KeyGameActions[action] == KeyCodes::RMOUSE)
			return m_Input->GetMouseKey(m_Input->m_KeyGameActions[action]);

		return m_Input->GetKey(m_Input->m_KeyGameActions[action]);
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
	if (m_Input != nullptr)
		return m_Input->GetMouseKey(key);
	return false;
}

glm::vec2 CInputManager::GetMousePosition()
{
	if (m_Input != nullptr)
		return m_Input->GetMousePosition();
	return glm::vec2();
}

glm::vec2 CInputManager::CalcualteMouseMove()
{
	if (m_Input != nullptr)
		return m_Input->CalcualteMouseMove();

	return glm::vec2();
}
