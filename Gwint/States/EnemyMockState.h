#pragma once
#include "State.h"
#include "../Player.h"
#include "../GameLine.h"
#include "../../GameEngine/Input/InputManager.h"
#include "../Cards/GameCards.h"

class CEnemyStateMock : public CState
{
public:
	CEnemyStateMock(Player& player, CInputManager* input_manager, std::map<LineTypes, GameLine>& GameLines)
		: player(player)
		, input_manager(input_manager)
		, GameLines(GameLines)
	{
		m_GameStateType = GameStates::ENEMY_ROUND;
	}

	virtual void KeyInputDir() override
	{


	}
	virtual void KeyInputConfirm() override {}
	virtual GameStates::State KeyInputRetrun() override
	{
		std::cout << "Enemy move... end" << std::endl;
		
		if (!player.cards_in_hand.empty())
		{
			auto size = player.cards_in_hand.size();
			auto i = rand() % size;
			auto c = player.cards_in_hand[i];
			GameLines[c.type].AddCard(c);
			player.cards_in_hand.erase(player.cards_in_hand.begin() + i);
			return GameStates::END_ROUND;
		}		
		return GameStates::END_ROUND;
	}
	virtual std::list<SCard> CardRender() override
	{
		//renderer_quad->m_WorldTransform.SetScale(glm::vec3(-1, 1, 1));
		//renderer_quad->m_WorldTransform.SetPosition(glm::vec3(0, 0, -2));

		std::list<SCard> cards;
		float x = -0.47f;

		int a = static_cast<int>(player.cards_in_hand.size()) - 1;
		int b = -1 * a;

		if (selected_nr < b)
			selected_nr = b;

		if (selected_nr > 0)
			selected_nr = 0;

		int nr = selected_nr;

		for (auto card : player.cards_in_hand)
		{
			card.position = glm::vec3(0, 0, -1.8);
			card.scale = select_card_size;
			if (nr == 0)
				card.scale *= 1.3f;
			card.position.x += x * nr++;
			cards.push_back(card);
		}

		return cards;
	}
	int selected_nr = 0;
	Player& player;
	CInputManager* input_manager;
	std::map<LineTypes, GameLine>& GameLines;

	int moves = 0;
};