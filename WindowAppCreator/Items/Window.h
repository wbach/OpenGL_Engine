#pragma once
#include "./Item.h"
#include "IItemBuidler.h"
#include <functional>
#include <unordered_map>
#include <list>

class Window;

typedef std::shared_ptr<Window> WindowPtr;
typedef std::unordered_map<unsigned int, std::function<void()>> FunctionsMap;

template<typename T, typename... Args>
WindowPtr CreateWindowPtr(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

class Window : public virtual Item
{	
public:
	virtual ~Window() {}

public:
	void RegisterAction(ActionsType, unsigned int, std::function<void()>);
	virtual void AddChild(ItemPtr) override;
protected:
	virtual ItemMessage Init() = 0;

protected:
	FunctionsMap actions[ActionsType::COUNT];	
};
