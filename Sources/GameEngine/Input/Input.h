#pragma once
#include "KeyCodes.h"
#include "GameActions.h"
#include "Types.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <set>

class CApi;

class CInput 
{
public:
	CInput();
	void SetDefaultKeys();
	virtual bool GetKeyDown(KeyCodes::Type i);
	virtual bool GetKeyUp(KeyCodes::Type  i);
	virtual bool GetKey(KeyCodes::Type i) = 0;
	virtual std::vector<KeyCodes::Type> GetKey() = 0;
	virtual std::vector<KeyCodes::Type> GetKeyUp() = 0;
	virtual std::vector<KeyCodes::Type> GetKeyDown() = 0;

	virtual bool GetMouseKeyDown(KeyCodes::Type key) = 0;
	virtual bool GetMouseKeyUp(KeyCodes::Type key) = 0;
	virtual bool GetMouseKey(KeyCodes::Type key) = 0;
	virtual vec2 CalcualteMouseMove() = 0;
	virtual vec2 GetMousePosition() = 0;

	virtual void SetCursorPosition(int x, int y) = 0;
	virtual void SetKeyToBuffer(int key, bool value) = 0;
	virtual void ClearKeyBuffer() = 0;

	virtual void GetPressedKeys() = 0;
	void CheckReleasedKeys();

	std::vector<char> GetCharKey(KeyCodes::Type key);
	std::vector<char> GetCharKey();

public:
    KeyCodes::Type keyGameActions[GameActions::COUNT];
    CApi* api;

protected:
	std::set<KeyCodes::Type> keyBuffer;

private:
	bool IsPressed(KeyCodes::Type key);

private:
	std::unordered_map<KeyCodes::Type, char> smallCharKeys;
	std::unordered_map<KeyCodes::Type, char> bigCharKeys;
    std::list<std::pair<KeyCodes::Type, bool>> pressedKeys;	
};
