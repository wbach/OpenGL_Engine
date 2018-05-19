#pragma once
#include "Types.h"
#include <memory>
#include <vector>

class CGameObject;

namespace GameEngine
{
	class Scene;

	struct IRenderer
	{
		virtual ~IRenderer();
		virtual void Init() = 0;
		virtual void ReloadShaders() = 0;
		virtual void Subscribe(CGameObject* gameObject);
		virtual void UnSubscribe(CGameObject* gameObject);
		virtual void UnSubscribeAll();
	};

	typedef std::unique_ptr<IRenderer> IRendererPtr;
	typedef std::vector<IRendererPtr>   IRenderersPtrVec;
} // GameEngine
