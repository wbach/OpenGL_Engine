#include "Input.h"

CInput::CInput()
{
	SetDefaultKeys();
}

void CInput::SetDefaultKeys()
{
    keyGameActions[GameActions::MOVE_FORWARD] = KeyCodes::W;
    keyGameActions[GameActions::MOVE_BACKWARD] = KeyCodes::S;
    keyGameActions[GameActions::MOVE_LEFT] = KeyCodes::Q;
    keyGameActions[GameActions::MOVE_RIGHT] = KeyCodes::R;
    keyGameActions[GameActions::TURN_LEFT] = KeyCodes::A;
    keyGameActions[GameActions::TURN_RIGHT] = KeyCodes::D;
    keyGameActions[GameActions::JUMP] = KeyCodes::SPACE;
    keyGameActions[GameActions::ATTACK_1] = KeyCodes::LMOUSE;
    keyGameActions[GameActions::ATTACK_2] = KeyCodes::RMOUSE;
    keyGameActions[GameActions::ATTACK_3] = KeyCodes::Z;
    keyGameActions[GameActions::SPELL_1] = KeyCodes::W;
    keyGameActions[GameActions::SPELL_2] = KeyCodes::W;
    keyGameActions[GameActions::SPELL_3] = KeyCodes::W;
    keyGameActions[GameActions::SPELL_4] = KeyCodes::W;
    keyGameActions[GameActions::GUI_STATS_WINDOW] = KeyCodes::C;
    keyGameActions[GameActions::GUI_INVENTORY_WINDOW] = KeyCodes::I;
    keyGameActions[GameActions::GUI_PAUSE_MENU_WINDOW] = KeyCodes::ESCAPE;
    keyGameActions[GameActions::ITEM_1] = KeyCodes::W;
    keyGameActions[GameActions::ITEM_2] = KeyCodes::W;
    keyGameActions[GameActions::ITEM_3] = KeyCodes::W;
    keyGameActions[GameActions::ITEM_4] = KeyCodes::W;
    keyGameActions[GameActions::ITEM_5] = KeyCodes::W;
    keyGameActions[GameActions::ITEM_6] = KeyCodes::W;
    keyGameActions[GameActions::WORLD_MAP] = KeyCodes::M;
}

bool CInput::GetKeyDown(KeyCodes::Type  i)
{	
	if (GetKey(i))
	{
        for (auto& k : pressedKeys)
		{
			if (k.first == i)
			{
				k.second = true;
				return false;
			}
		}

        pressedKeys.push_back({ i, true });
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
    for (std::list<std::pair<KeyCodes::Type, bool>>::iterator iter = pressedKeys.begin(); iter != pressedKeys.end(); ++iter)
	{
		if (iter->second)
		{
			iter->second = false;
		}
		else
		{
            pressedKeys.erase(iter);
			return;
		}
	}
}
