#pragma once
#include "State.h"
#include "../Player.h"
#include "../GameLine.h"
#include "../Network/SDLClientGetway.h"
#include "../Messages/Messages.h"
#include <Input/InputManager.h>

class CChooseCardState : public CState
{
public:
	CChooseCardState(Player& player, CInputManager* input_manager, std::map<LineTypes, GameLine>& GameLines)
		: player(player)
		, input_manager(input_manager)
		, GameLines(GameLines)
	{
		m_GameStateType = GameStates::CHOOSE_CARDS_START;
	}

	virtual void KeyInputDir() override 
	{
		if (input_manager == nullptr)
			return;

		if (input_manager->GetKeyDown(KeyCodes::D))
		{
			selected_nr++;
		}

		if (input_manager->GetKeyDown(KeyCodes::A))
		{
			selected_nr--;
		}
	}
	virtual void KeyInputConfirm() override {}
	virtual GameStates::State KeyInputRetrun() override
	{
		if (input_manager->GetKeyDown(KeyCodes::ENTER))
		{
				//remote_swap card
				int index = selected_nr*-1;
				auto& card = player.cards_in_hand[index];

				GwentMessages::SwapCardMessage msg;
				msg.index = index;
				msg.card_name = card.name;

				Log("Sending swap card req : \n" + msg.ToString());
				SDLClientGetway::Instance().SendMessage(msg.ToString());							
			

			//Local swap card
			/*auto card = player.cards_in_hand[selected_nr*-1];
			auto i = rand() % player.cards_in_deck.size();
			player.cards_in_hand[selected_nr*-1] = player.cards_in_deck[i];
			player.cards_in_deck[i] = card;
			swaped_cards++;*/

			
			if( swaped_cards == 2)
				return GameStates::END_ROUND;

		}
		if (input_manager->GetKeyDown(KeyCodes::C))
		{
			SDLClientGetway::Instance().SendMessage("<SWAP_CARD><cancel/></ SWAP_CARD>");
		}
		return GameStates::NONE;
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
			{
				card.scale *= 1.3f;
				card.position.z = -1.79f;
			}
				
			card.position.x += x * nr++;
			cards.push_back(card);
		}

		return cards;
	}
	int selected_nr = 0;
	Player& player;
	CInputManager* input_manager;
	int swaped_cards = 0;
	std::map<LineTypes, GameLine>& GameLines;
};