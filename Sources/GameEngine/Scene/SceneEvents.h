#pragma once
#include "Types.h"
#include <functional>

namespace GameEngine
{
	enum SceneEventType
	{
		LOAD_NEXT_SCENE,
		LOAD_SCENE_BY_NAME,
		LOAD_SCENE_BY_ID
	};

	struct SceneEvent
	{
		SceneEventType type;
		std::string name;
		uint32 id;
	};

	typedef std::function<void(const SceneEvent&)> AddEvent;
}
