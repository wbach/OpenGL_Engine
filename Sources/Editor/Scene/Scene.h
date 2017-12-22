#pragma once
#include "../GameEngine/Scene/Scene.hpp" 

class CEngine;

class MainScene : public CScene
{
public:
	MainScene(CEngine& engine);
	virtual ~MainScene() override;
	virtual int		Initialize();
	virtual void	PostInitialize() {}
	virtual int		Update();

private:
	std::vector<CGameObject*> terrains;
	double timeClock = 0;
	CEngine& engine;
};