#pragma once
#include "../GameObject.h"
#include "../../Resources/ResourceManager.h"
#include "../../Resources/Models/Model.h"
#include <vector>

struct CGrass : public CGameObject
{
	virtual ~CGrass() {}
	CGrass()
	{ 	
	
	}
	CModel *model;
	std::vector<float> grassvertex;
};