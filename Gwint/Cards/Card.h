#pragma once
#include <Resources/Textures/Texture.h>
#include <Utils/Utils.h>

static const glm::vec3 hand_card_size(-0.2, -0.5, 1);
static const glm::vec3 select_card_size(-0.2, -0.5, 1);
static const glm::vec3 table_card_size(select_card_size / 4.5f);

enum class LineTypes
{
	CLOSE_COMBAT = 0,
	ARCHEER,
	BALIST
};

enum class Skills
{
	MoraleBoost, // daje wszystkim +1 oprocz siebie
	ScorchALL, // niszczy najsilniejsza na stole
	Scorch, // niszczy najsilniejsza w swojej lini
	Spy, // szpieg
	Hero, // efekty nie dizalaja
	TightBond, // podwojone gdy obok jest 2 karta
	Medic, // ozywianie
	Agile, // archer/ close combat
	Muster //Przywo³anie z tali 
};

struct SCard
{
	SCard() 
	{
		static int s_Id = 0;
		m_Id = s_Id++;
	}
	int m_Id = 0;
	std::string name;
	CTexture* texture = nullptr;
	//read xml texture path and after loading
	std::string texture_path;
	glm::vec3 position;
	glm::vec3 scale;
	LineTypes type;
	int strength = 0;
	std::vector<Skills> skills;

	bool FindSkill(Skills skill) const
	{
		return std::find(std::begin(skills), std::end(skills), skill) != std::end(skills);
	}
};