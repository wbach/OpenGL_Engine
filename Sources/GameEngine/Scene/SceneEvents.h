#pragma once
#include "Types.h"
#include <functional>

namespace GameEngine
{
	enum SceneEventType
	{
		NONE,
		LOAD_NEXT_SCENE,
		LOAD_PREVIOUS_SCENE,
		LOAD_SCENE_BY_NAME,
		LOAD_SCENE_BY_ID
	};

	struct SceneEvent
	{
		SceneEvent(SceneEventType t, uint32 id)
			: SceneEvent(t, "", id)
		{
		}
		SceneEvent(SceneEventType t = SceneEventType::NONE, const std::string& n = "", uint32 i = 0)
			: type(t)
			, name(n)
			, id(i)
		{}

		SceneEventType type;
		std::string name;
		uint32 id;
	};

	typedef std::function<void(const SceneEvent&)> AddEvent;
}
