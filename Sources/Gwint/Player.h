#pragma once
#include "Cards/Card.h"
#include <vector>

class Player
{
public:
	std::vector<uint> indexes; //Send randed index
	std::vector<SCard> cards_in_deck;
	std::vector<SCard> cards_in_hand;
	std::vector<SCard> cards_on_table;
	std::vector<SCard> cards_in_trash;
	uint swaped_cards = 0;
	//Cards
	void TakeCardsStart()
	{
		if (cards_in_hand.size() >= 10)
			return;

		for (uint x = 0; x < 10; x++)
		{
			TakeSingleCard();
		}
	}
	SCard TakeSingleCard()
	{
		if (cards_in_deck.empty())
			return SCard();

		auto i = rand() % cards_in_deck.size();
		cards_in_hand.push_back(cards_in_deck[i]);
		cards_in_deck.erase(cards_in_deck.begin() + i);
		indexes.push_back(i);
	}
private:
	
};