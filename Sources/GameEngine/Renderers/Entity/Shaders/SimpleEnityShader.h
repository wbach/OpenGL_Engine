#pragma once
#include "EntityShader.h"

class CSimpleEntityShader : public CEntityShader
{
public:
	CSimpleEntityShader(GameEngine::IGraphicsApiPtr graphicsApi);
	virtual void Init() override;
};
