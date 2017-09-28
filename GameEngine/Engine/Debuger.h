#pragma once
#include "../Input/InputManager.h"
#include <functional>
#include <unordered_map>

typedef std::unordered_map<KeyCodes::Type, std::function<void()>> Actions;

class CDebuger
{
public:
	CDebuger(CInputManager& input);
	void TurnOnOff();
	void Execute();
	void Render();
	void AddAction(KeyCodes::Type key, std::function<void()> action);

private:
	void RenderAllasGrid();	
	void Keys();
	void Init();

private:
	bool isInit = false;
	bool isEnabled = false;
	bool renderAsGrid = false;
	CInputManager& inputManager;
	Actions actions;
};