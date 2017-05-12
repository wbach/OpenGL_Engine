#include "Input.h"

CInput::CInput()
{
	SetDefaultKeys();
}

void CInput::SetDefaultKeys()
{
	m_KeyGameActions[GameActions::MOVE_FORWARD] = KeyCodes::W;
	m_KeyGameActions[GameActions::MOVE_BACKWARD] = KeyCodes::S;
	m_KeyGameActions[GameActions::MOVE_LEFT] = KeyCodes::Q;
	m_KeyGameActions[GameActions::MOVE_RIGHT] = KeyCodes::R;
	m_KeyGameActions[GameActions::TURN_LEFT] = KeyCodes::A;
	m_KeyGameActions[GameActions::TURN_RIGHT] = KeyCodes::D;
	m_KeyGameActions[GameActions::JUMP] = KeyCodes::SPACE;
	m_KeyGameActions[GameActions::ATTACK_1] = KeyCodes::LMOUSE;
	m_KeyGameActions[GameActions::ATTACK_2] = KeyCodes::RMOUSE;
	m_KeyGameActions[GameActions::ATTACK_3] = KeyCodes::Z;
	m_KeyGameActions[GameActions::SPELL_1] = KeyCodes::W;
	m_KeyGameActions[GameActions::SPELL_2] = KeyCodes::W;
	m_KeyGameActions[GameActions::SPELL_3] = KeyCodes::W;
	m_KeyGameActions[GameActions::SPELL_4] = KeyCodes::W;
	m_KeyGameActions[GameActions::GUI_STATS_WINDOW] = KeyCodes::C;
	m_KeyGameActions[GameActions::GUI_INVENTORY_WINDOW] = KeyCodes::I;
	m_KeyGameActions[GameActions::GUI_PAUSE_MENU_WINDOW] = KeyCodes::ESCAPE;
	m_KeyGameActions[GameActions::ITEM_1] = KeyCodes::W;
	m_KeyGameActions[GameActions::ITEM_2] = KeyCodes::W;
	m_KeyGameActions[GameActions::ITEM_3] = KeyCodes::W;
	m_KeyGameActions[GameActions::ITEM_4] = KeyCodes::W;
	m_KeyGameActions[GameActions::ITEM_5] = KeyCodes::W;
	m_KeyGameActions[GameActions::ITEM_6] = KeyCodes::W;
	m_KeyGameActions[GameActions::WORLD_MAP] = KeyCodes::M;
}

bool CInput::GetKeyDown(KeyCodes::Type  i)
{	
	if (GetKey(i))
	{
		for (auto& k : m_PressedKeys)
		{
			if (k.first == i)
			{
				k.second = true;
				return false;
			}
		}

		m_PressedKeys.push_back({ i, true });
		return true;
	}	
	return false;
}

bool CInput::GetKeyUp(KeyCodes::Type i)
{
	//m_PressedKeys.erase(i);	
	return false;
}

void CInput::CheckReleasedKeys()
{
	for (std::list<std::pair<KeyCodes::Type, bool>>::iterator iter = m_PressedKeys.begin(); iter != m_PressedKeys.end(); ++iter)
	{
		if (iter->second)
		{
			iter->second = false;
		}
		else
		{
			m_PressedKeys.erase(iter);
			return;
		}
	}
}
