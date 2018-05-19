#pragma once
#include "Utils.h"
#include <memory>

namespace GameEngine
{
	class GuiElement
	{
	public:
		virtual ~GuiElement() {}
		virtual void Init() = 0;
		virtual void Render() = 0;
		virtual void UnSubscribeAll() = 0;
		virtual void ReloadShaders() = 0;
		vec2 position;
	};

	typedef std::shared_ptr<GuiElement> GuiElementPtr;
} // GameEngine
