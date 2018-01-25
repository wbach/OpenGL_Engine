#include "Input.h"
#include <algorithm>

CInput::CInput()
{
	SetDefaultKeys();

	smallCharKeys =
	{
		{ KeyCodes::Q, 'q' },
		{ KeyCodes::W, 'w' },
		{ KeyCodes::E, 'e' },
		{ KeyCodes::R, 'r' },
		{ KeyCodes::T, 't' },
		{ KeyCodes::Y, 'y' },
		{ KeyCodes::U, 'u' },
		{ KeyCodes::I, 'i' },
		{ KeyCodes::O, 'o' },
		{ KeyCodes::O, 'p' },
		{ KeyCodes::A, 'a' },
		{ KeyCodes::S, 's' },
		{ KeyCodes::D, 'd' },
		{ KeyCodes::F, 'f' },
		{ KeyCodes::G, 'g' },
		{ KeyCodes::H, 'h' },
		{ KeyCodes::J, 'j' },
		{ KeyCodes::K, 'k' },
		{ KeyCodes::L, 'l' },
		{ KeyCodes::Z, 'z' },
		{ KeyCodes::X, 'x' },
		{ KeyCodes::C, 'c' },
		{ KeyCodes::V, 'v' },
		{ KeyCodes::B, 'b' },
		{ KeyCodes::N, 'n' },
		{ KeyCodes::M, 'm' }
	};

	bigCharKeys =
	{
		{ KeyCodes::Q, 'Q' },
		{ KeyCodes::W, 'W' },
		{ KeyCodes::E, 'E' },
		{ KeyCodes::R, 'R' },
		{ KeyCodes::T, 'T' },
		{ KeyCodes::Y, 'Y' },
		{ KeyCodes::U, 'U' },
		{ KeyCodes::I, 'I' },
		{ KeyCodes::O, 'O' },
		{ KeyCodes::O, 'P' },
		{ KeyCodes::A, 'A' },
		{ KeyCodes::S, 'S' },
		{ KeyCodes::D, 'D' },
		{ KeyCodes::F, 'F' },
		{ KeyCodes::G, 'G' },
		{ KeyCodes::H, 'H' },
		{ KeyCodes::J, 'J' },
		{ KeyCodes::K, 'K' },
		{ KeyCodes::L, 'L' },
		{ KeyCodes::Z, 'Z' },
		{ KeyCodes::X, 'X' },
		{ KeyCodes::C, 'C' },
		{ KeyCodes::V, 'V' },
		{ KeyCodes::B, 'B' },
		{ KeyCodes::N, 'N' },
		{ KeyCodes::M, 'M' }
	};
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
	if (!GetKey(i))
		return false;

	if (IsPressed(i))
		return false;

    pressedKeys.push_back({ i, true });
	return true;
}

bool CInput::GetKeyUp(KeyCodes::Type i)
{
	//m_PressedKeys.erase(i);	
	return false;
}

std::vector<KeyCodes::Type> CInput::GetKeyDown()
{
	auto key = GetKey();

	if (key.empty())
		return {};

	std::vector<KeyCodes::Type> result;

	for (auto i : key)
	{
		if (IsPressed(i))
			continue;

		pressedKeys.push_back({ i, true });
		result.push_back(i);
	}
	
	return result;
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

std::vector<char> CInput::GetCharKey(KeyCodes::Type key)
{
	return {};
}

std::vector<char> CInput::GetCharKey()
{
	std::unordered_map<KeyCodes::Type, char> * map = &smallCharKeys;
	std::vector<char> result;

	if(GetKey(KeyCodes::LSHIFT))
		map = &bigCharKeys;

	auto key = GetKeyDown();

	if (key.empty())
		return result;

	for (auto k : key)
	{
		if (map->count(k) == 0)
			continue;

		char c = (*map)[k];
		result.push_back(c);
	}
	return result;
}

bool CInput::IsPressed(KeyCodes::Type key)
{
	for (auto& k : pressedKeys)
	{
		if (k.first == key)
		{
			k.second = true;
			return true;
		}
	}
	return false;
}
