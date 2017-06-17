#pragma once
#include "Utils.h"

class CGuiElement
{
public:
	virtual ~CGuiElement() = 0;
	virtual void Init() = 0;
	virtual void Render() = 0;
	glm::vec2 position;
};
