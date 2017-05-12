#pragma once
#include "GameLine.h"
#include "Renderer.h"
#include "GameScene.h"
#include "Cards/GameCards.h"
#include <Engine/Engine.h>

class CGwintGame
{
public:
	void Start();
private:
	void NetworkStartProcedure();
	CGwitRenderer* renderer;
	GwintGameScene* scene;
	CEngine engine;
};