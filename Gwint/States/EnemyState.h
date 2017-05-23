#pragma once
#include "State.h"
#include "../Player.h"
#include "../GameLine.h"
#include "../../GameEngine/Input/InputManager.h"
#include "../Cards/GameCards.h"
#include "../Network/NetworkStates.h"

class CEnemyState : public CState
{
public:
	CEnemyState(Player& player, NetworkState& networkState)
		: player(player)
		, networkState(networkState)
	{
		m_GameStateType = GameStates::ENEMY_ROUND;
	}

	virtual void KeyInputDir() override
	{


	}
	virtual void KeyInputConfirm() override {}
	virtual GameStates::State KeyInputRetrun() override
	{		
		if (networkState == NetworkState::PLAYER_TURN)
			return GameStates::END_ROUND;
		return GameStates::NONE;
	}
	virtual std::list<SCard> CardRender() override
	{
		std::list<SCard> cards;
		float card_width = 0.1f;


		while (card_width*player.cards_in_hand.size() > 1.f)
		{
			card_width -= 0.001f;
		}
		card_width *= -1;
		int a = static_cast<int>(player.cards_in_hand.size()) - 1;
		int b = -1 * a;


		int nr = 0;

		for (auto card : player.cards_in_hand)
		{
			card.position = glm::vec3(0, -0.7, -1.8);
			card.scale = table_card_size;
	/*		if (nr == -selected_nr) {
				card.scale *= 1.3f;
				card.position.z = -1.79f;
			}*/
			card.position.x += -card_width * nr++ - 0.4f;
			cards.push_back(card);
		}

		return cards;
	}

	Player& player;
	NetworkState& networkState;
};