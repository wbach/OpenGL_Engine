#pragma once
#include "State.h"
#include "../Player.h"

class CPlayerMoveState : public CState
{
public:
	CPlayerMoveState(Player& player, CInputManager* input_manager, std::map<LineTypes, GameLine>& GameLines)
		: player(player)
		, input_manager(input_manager)
		, GameLines(GameLines)
	{
		m_GameStateType = GameStates::PLAYER_ROUND;
	}

	virtual void KeyInputDir() override
	{
		if (input_manager == nullptr)
			return;

		if (input_manager->GetKeyDown(KeyCodes::D))
		{
			selected_nr--;
		}

		if (input_manager->GetKeyDown(KeyCodes::A))
		{
			selected_nr++;
		}
	}
	virtual void KeyInputConfirm() override {}
	virtual GameStates::State KeyInputRetrun() override
	{
		
		if (input_manager->GetKeyDown(KeyCodes::SPACE))
		{
			SDLClientGetway::Instance().SendMessage("PLAYER_PASS");
			return GameStates::NONE;
		}

		if (input_manager->GetKeyDown(KeyCodes::ENTER))
		{
			auto index = selected_nr*-1;
			auto card = player.cards_in_hand[index];
			GwentMessages::PushCardMessage push_card_mag;
			push_card_mag.card_name = card.name;
			push_card_mag.index = index;
			push_card_mag.player = GwentMessages::Player::PLAYER;
			push_card_mag.texturePath = card.texture_path;
			push_card_mag.type = static_cast<int>(card.type);
			Log("Sending push card req : \n" + push_card_mag.ToString());
			SDLClientGetway::Instance().SendMessage(push_card_mag.ToString());
		}
		return GameStates::NONE;
	}
	virtual std::list<SCard> CardRender() override
	{
		//renderer_quad->m_WorldTransform.SetScale(glm::vec3(-1, 1, 1));
		//renderer_quad->m_WorldTransform.SetPosition(glm::vec3(0, 0, -2));

		std::list<SCard> cards;
		float card_width = 0.1f;


		while (card_width*player.cards_in_hand.size() > 1.f)
		{
			card_width -= 0.001f;
		}
		card_width *= -1;
		int a = static_cast<int>(player.cards_in_hand.size()) - 1;
		int b = -1 * a;

		if (selected_nr < b)
			selected_nr = b;

		if (selected_nr > 0)
			selected_nr = 0;

		int nr = 0;

		for (auto card : player.cards_in_hand)
		{
			card.position = glm::vec3(0, -0.7, -1.8);
			card.scale = table_card_size;
			if (nr == -selected_nr) {
				card.scale *= 1.3f;
				card.position.z = -1.79f;
			}
			card.position.x += -card_width * nr++ - 0.4f;
			cards.push_back(card);
		}
		//cards.reverse();
		return cards;
	}
	int selected_nr = 0;
	Player& player;
	CInputManager* input_manager;
	std::map<LineTypes, GameLine>& GameLines;
};