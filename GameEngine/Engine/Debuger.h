#pragma once
#include "../Input/InputManager.h"
#include <functional>
#include <unordered_map>

class CGuiText;
class CGUIRenderer;
typedef std::unordered_map<KeyCodes::Type, std::function<void()>> Actions;

class CDebuger
{
public:
	CDebuger(CInputManager& input);
	void TurnOnOff();
	void Execute();
	void Render();

	void Init();
	void AddAction(KeyCodes::Type key, std::function<void()> action);
	void SetGuiRenderer(CGUIRenderer* renderer);

private:
	void RenderAllasGrid();	
	void Keys();
	void UpdateCommandText();
	bool isSetGuiRenderer() const;

private:
	bool isInit = false;
	bool isEnabled = false;
	bool renderAsGrid = false;
	CInputManager& inputManager;
	CGUIRenderer* gui_renderer;
	Actions actions;

	std::list<std::string> commandsHistory;
	CGuiText* guiText = nullptr;
};
