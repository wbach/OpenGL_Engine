#include "GameCards.h"
#include <Resources/ResourceManager.h>

GameCards & GameCards::Instance()
{
	static GameCards game_cards;
	return game_cards;
}

void GameCards::LoadCards()
{
	decks[DeckType::HEROS] = CardReader::ReadCardFromXML("../Data/Gwint/Cards/Cards.xml");
	decks[DeckType::NORTH] = CardReader::ReadCardFromXML("../Data/Gwint/Cards/KrolestwaPolnocy.xml");
}

bool GameCards::LoadCards(DeckType deck, const std::string & xml_str)
{
	decks[deck] = CardReader::ReadCardFromString(xml_str);
	return !decks[deck].empty();
}

void GameCards::LoadTexturesInCards(CResourceManager & manager, bool now)
{
	for (auto& cards : decks)
	{
		for (auto& card : cards.second)
		{
			if (!now)
				card.texture = manager.GetTextureLaoder().LoadTexture(card.texture_path);
			else
				card.texture = manager.GetTextureLaoder().LoadTextureImmediately(card.texture_path);
		}
	}
}

const std::map<DeckType, std::vector<SCard>>& GameCards::GetCards() const
{
	return decks;
}

GameCards::GameCards()
{
}
