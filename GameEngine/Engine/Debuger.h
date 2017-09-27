#pragma once
#include "../Input/InputManager.h"

class CDebuger
{
public:
	CDebuger(CInputManager& input);
	void TurnOnOff();
	void Execute();
	void Render();

private:
	void RenderAllasGrid();	
	void Keys();
	void Init();

private:
	bool isInit = false;
	bool isEnabled = false;
	bool renderAsGrid = false;
	CInputManager& inputManager;
};