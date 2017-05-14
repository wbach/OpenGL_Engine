#pragma once
#include "../GameObject.h"
#include <vector>

class CModel;

struct SGrass : public CGameObject
{
	virtual ~SGrass() {}
	SGrass()
	{ 	
	
	}
	CModel *model;
	std::vector<float> grassvertex;
};