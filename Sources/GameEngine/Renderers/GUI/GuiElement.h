#pragma once
#include "Utils.h"
#include <memory>

class CGuiElement
{
public:
    virtual ~CGuiElement() = 0;
    virtual void Init()    = 0;
    virtual void Render()  = 0;
	virtual void UnSubscribeAll() = 0;
    vec2 position;
};

typedef std::shared_ptr<CGuiElement> CGuiElementPtr;
