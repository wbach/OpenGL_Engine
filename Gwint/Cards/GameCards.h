#pragma once
#include "CardReader.h"
#include <map>

class CResourceManager;

enum class DeckType
{
	HEROS,
	NORTH
};

class GameCards
{
public:
	static GameCards& Instance();

	void LoadCards();

	bool LoadCards(DeckType deck, const std::string& xml_str);

	void LoadTexturesInCards(CResourceManager& manager, bool now = false);

	const std::map<DeckType, std::vector<SCard>>& GetCards() const;
private:
	GameCards();
	std::map<DeckType, std::vector<SCard>> decks;
};