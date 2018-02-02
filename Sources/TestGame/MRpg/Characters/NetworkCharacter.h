#pragma once
#include "../../../Common/Controllers/IController.h"
#include "../../../GameEngine/Objects/RenderAble/Entity/EntityWrapper.h"
#include <unordered_map>

namespace MmmoRpg
{
	typedef std::unordered_map<common::Controllers::Types, common::Controllers::IController> ControllersMap;

	class NetworkCharacter 
	{
	public:
		NetworkCharacter() {}

	private:
		CEntityWrapper enitityWrapper_;
		ControllersMap controllers_;
	};
} // MmmoRpg